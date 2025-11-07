//client_cpp_sdk.h
void subscribeDurable(UaString& sUrl, SessionSecurityInfo& sessionSecurityInfo)
{
    printf("\n\n****************************************************************\n");
    printf("** Try to create a subscription\n");
    if (g_pUaSession == NULL)
    {
        printf("** Error: Server not connected\n");
        printf("****************************************************************\n");
        return;
    }

    UaStatus             status;
    UaSubscription*      pUaSubscription = NULL;
    SubscriptionSettings subscriptionSettings;
    subscriptionSettings.publishingInterval = 500;
    subscriptionSettings.lifetimeCount = 20;
    ServiceSettings      serviceSettings;

    /*********************************************************************
    Create a Subscription
    **********************************************************************/
    status = g_pUaSession->createSubscription(
        serviceSettings,        // Use default settings
        g_pCallback,            // Callback object
        0,                      // We have only one subscription, handle is not needed
        subscriptionSettings,   // general settings
        OpcUa_True,             // Publishing enabled
        &pUaSubscription);      // Returned Subscription instance
                                /*********************************************************************/
    if (status.isBad())
    {
        printf("** Error: UaSession::createSubscription failed [ret=%s] **\n", status.toString().toUtf8());
    }
    else
    {
        CallIn callRequest;
        CallOut results;
        callRequest.objectId.setNodeId(OpcUaId_Server, 0);
        callRequest.methodId.setNodeId(OpcUaId_Server_SetSubscriptionDurable, 0);

        UaVariant uvInput;
        callRequest.inputArguments.create(2);
        // SubscriptionId
        uvInput.setUInt32(pUaSubscription->subscriptionId());
        uvInput.copyTo(&callRequest.inputArguments[0]);
        // LifeTimeInHours
        uvInput.setUInt32(20);
        uvInput.copyTo(&callRequest.inputArguments[1]);

        status = g_pUaSession->call(
            serviceSettings,
            callRequest,
            results);
        if (status.isGood())
        {
            status = results.callResult.statusCode();
        }
        if (status.isBad())
        {
            printf("** Error: SetDurableSubscription failed [ret=%s] **\n", status.toString().toUtf8());
            return;
        }

        printf("****************************************************************\n");
        printf("** Try to create monitored items\n");

        OpcUa_UInt32                  i;
        OpcUa_UInt32                  count;
        UaMonitoredItemCreateRequests monitoredItemCreateRequests;
        UaMonitoredItemCreateResults  monitoredItemCreateResults;

        // Initialize IN parameter monitoredItemCreateRequests
        count = g_VariableNodeIds.length();
        monitoredItemCreateRequests.create(count + 1); // We create also an event monitored item
        for (i = 0; i<count; i++)
        {
            g_VariableNodeIds[i].copyTo(&monitoredItemCreateRequests[i].ItemToMonitor.NodeId);
            monitoredItemCreateRequests[i].ItemToMonitor.AttributeId = OpcUa_Attributes_Value;
            monitoredItemCreateRequests[i].MonitoringMode = OpcUa_MonitoringMode_Reporting;
            monitoredItemCreateRequests[i].RequestedParameters.ClientHandle = i + 1;
            monitoredItemCreateRequests[i].RequestedParameters.SamplingInterval = 50;
            monitoredItemCreateRequests[i].RequestedParameters.QueueSize = 1000000;
            monitoredItemCreateRequests[i].RequestedParameters.DiscardOldest = OpcUa_True;

            // Sample code for creation of data change filter
            /*
            OpcUa_DataChangeFilter* pDataChangeFilter = NULL;
            OpcUa_EncodeableObject_CreateExtension(
            &OpcUa_DataChangeFilter_EncodeableType,
            &monitoredItemCreateRequests[i].RequestedParameters.Filter,
            (OpcUa_Void**)&pDataChangeFilter);
            if ( pDataChangeFilter )
            {
            // Deadband setting
            pDataChangeFilter->DeadbandType = OpcUa_DeadbandType_Absolute;
            pDataChangeFilter->DeadbandValue = 0.1; // 0.1% of last value
            // Trigger setting (default is StatusValue)
            pDataChangeFilter->Trigger      = OpcUa_DataChangeTrigger_StatusValue;
            }
            */
        }

        // ------------------------------------------------------
        // ------------------------------------------------------
        // 事件部分Set Event item
        monitoredItemCreateRequests[count].ItemToMonitor.NodeId.Identifier.Numeric = OpcUaId_Server;
        monitoredItemCreateRequests[count].ItemToMonitor.AttributeId = OpcUa_Attributes_EventNotifier;
        monitoredItemCreateRequests[count].MonitoringMode = OpcUa_MonitoringMode_Reporting;
        monitoredItemCreateRequests[count].RequestedParameters.ClientHandle = count + 1;
        monitoredItemCreateRequests[count].RequestedParameters.SamplingInterval = 0; // 0 is required by OPC UA spec
        monitoredItemCreateRequests[count].RequestedParameters.QueueSize = 0;

        UaEventFilter            eventFilter;
        UaSimpleAttributeOperand selectElement;
        UaContentFilter*         pContentFilter = NULL;
        UaContentFilterElement*  pContentFilterElement = NULL;
        UaFilterOperand*         pOperand = NULL;

        // -------------------------------------------------------------------------
        // 选择子句Define select clause with 5 event fields to be returned with every event
        // -------------------------------------------------------------------------
        selectElement.setBrowsePathElement(0, UaQualifiedName("Message", 0), 1);
        eventFilter.setSelectClauseElement(0, selectElement, 5);
        selectElement.setBrowsePathElement(0, UaQualifiedName("SourceName", 0), 1);
        eventFilter.setSelectClauseElement(1, selectElement, 5);
        selectElement.setBrowsePathElement(0, UaQualifiedName("EventType", 0), 1);
        eventFilter.setSelectClauseElement(2, selectElement, 5);
        selectElement.setBrowsePathElement(0, UaQualifiedName("Temperature", 3), 1);
        eventFilter.setSelectClauseElement(3, selectElement, 5);
        selectElement.setBrowsePathElement(0, UaQualifiedName("ConditionName", 0), 1);
        eventFilter.setSelectClauseElement(4, selectElement, 5);
        // -------------------------------------------------------------------------
        // Set the event field names requested in addition to Message
        g_pCallback->m_eventFields.clear();
        g_pCallback->m_eventFields.create(4);
        UaString sEventField;
        sEventField = "SourceName";
        sEventField.copyTo(&g_pCallback->m_eventFields[0]);
        sEventField = "EventType";
        sEventField.copyTo(&g_pCallback->m_eventFields[1]);
        sEventField = "Temperature";
        sEventField.copyTo(&g_pCallback->m_eventFields[2]);
        sEventField = "ConditionName";
        sEventField.copyTo(&g_pCallback->m_eventFields[3]);
        // -------------------------------------------------------------------------

        // -------------------------------------------------------------------------
        // 过滤条件字句 Define the where clause to filter the events sent to the client
        // Filter is ( (Severity > 100) AND (OfType(ControllerEventType) OR OfType(OffNormalAlarmType)) )
        // Represented as three ContentFilterElements
        // [0] [1] AND [2]
        // [1] Severity > 100
        // [2] [3] AND [4]
        // [3] OfType(ControllerEventType)
        // [4] OfType(OffNormalAlarmType)
        // -------------------------------------------------------------------------
        pContentFilter = new UaContentFilter;
        // [0] [1] AND [2] ------------------------------------------
        pContentFilterElement = new UaContentFilterElement;
        // Operator And
        pContentFilterElement->setFilterOperator(OpcUa_FilterOperator_And);
        // Operand 1 (Element)
        pOperand = new UaElementOperand;
        ((UaElementOperand*)pOperand)->setIndex(1);
        pContentFilterElement->setFilterOperand(0, pOperand, 2);
        // Operand 2 (Element)
        pOperand = new UaElementOperand;
        ((UaElementOperand*)pOperand)->setIndex(2);
        pContentFilterElement->setFilterOperand(1, pOperand, 2);
        pContentFilter->setContentFilterElement(0, pContentFilterElement, 5);
        // [1] Severity > 100  --------------------------------------
        pContentFilterElement = new UaContentFilterElement;
        // Operator GreaterThan
        pContentFilterElement->setFilterOperator(OpcUa_FilterOperator_GreaterThan);
        // Operand 1 (SimpleAttribute)
        pOperand = new UaSimpleAttributeOperand;
        ((UaSimpleAttributeOperand*)pOperand)->setBrowsePathElement(0, UaQualifiedName("Severity", 0), 1);;
        pContentFilterElement->setFilterOperand(0, pOperand, 2);
        // Operand 2 (Literal)
        pOperand = new UaLiteralOperand;
        ((UaLiteralOperand*)pOperand)->setLiteralValue(UaVariant((OpcUa_UInt16)100));
        pContentFilterElement->setFilterOperand(1, pOperand, 2);
        pContentFilter->setContentFilterElement(1, pContentFilterElement, 5);
        // [2] [3] OR [4] ------------------------------------------
        pContentFilterElement = new UaContentFilterElement;
        // Operator And
        pContentFilterElement->setFilterOperator(OpcUa_FilterOperator_Or);
        // Operand 1 (Element)
        pOperand = new UaElementOperand;
        ((UaElementOperand*)pOperand)->setIndex(3);
        pContentFilterElement->setFilterOperand(0, pOperand, 2);
        // Operand 2 (Element)
        pOperand = new UaElementOperand;
        ((UaElementOperand*)pOperand)->setIndex(4);
        pContentFilterElement->setFilterOperand(1, pOperand, 2);
        pContentFilter->setContentFilterElement(2, pContentFilterElement, 5);
        // [3] OfTpype(ControllerEventType) --------------------------
        pContentFilterElement = new UaContentFilterElement;
        // Operator OfType
        pContentFilterElement->setFilterOperator(OpcUa_FilterOperator_OfType);
        // Operand 1 (Literal)
        pOperand = new UaLiteralOperand;
        ((UaLiteralOperand*)pOperand)->setLiteralValue(UaVariant(UaNodeId(4000, 3))); // ControllerEventType
        pContentFilterElement->setFilterOperand(0, pOperand, 1);
        pContentFilter->setContentFilterElement(3, pContentFilterElement, 5);
        // [4] OfTpype(OffNormalAlarmType) --------------------------
        pContentFilterElement = new UaContentFilterElement;
        // Operator OfType
        pContentFilterElement->setFilterOperator(OpcUa_FilterOperator_OfType);
        // Operand 1 (Literal)
        pOperand = new UaLiteralOperand;
        ((UaLiteralOperand*)pOperand)->setLiteralValue(UaVariant(UaNodeId(OpcUaId_OffNormalAlarmType)));
        pContentFilterElement->setFilterOperand(0, pOperand, 1);
        pContentFilter->setContentFilterElement(4, pContentFilterElement, 5);
        //
        eventFilter.setWhereClause(pContentFilter);
        // -------------------------------------------------------------------------

        // Detach EventFilter to the create monitored item request
        eventFilter.toExtensionObject(monitoredItemCreateRequests[count].RequestedParameters.Filter, OpcUa_True);
        // ------------------------------------------------------
        // ------------------------------------------------------

        /*********************************************************************
        Call createMonitoredItems service
        **********************************************************************/
        status = pUaSubscription->createMonitoredItems(
            serviceSettings,               // Use default settings
            OpcUa_TimestampsToReturn_Both, // Select timestamps to return
            monitoredItemCreateRequests,   // monitored items to create
            monitoredItemCreateResults);   // Returned monitored items create result
                                           /*********************************************************************/
        if (status.isBad())
        {
            printf("** Error: UaSession::createMonitoredItems failed [ret=%s]\n", status.toString().toUtf8());
            printf("****************************************************************\n");
            return;
        }
        else
        {
            printf("** UaSession::createMonitoredItems result **********************\n");
            for (i = 0; i<count; i++)
            {
                UaNodeId node(monitoredItemCreateRequests[i].ItemToMonitor.NodeId);
                if (OpcUa_IsGood(monitoredItemCreateResults[i].StatusCode))
                {
                    printf("** Variable %s MonitoredItemId = %" OpcUa_PriUInt32 "\n", node.toString().toUtf8(), monitoredItemCreateResults[i].MonitoredItemId);
                }
                else
                {
                    printf("** Variable %s failed with error %s\n", node.toString().toUtf8(), UaStatus(monitoredItemCreateResults[i].StatusCode).toString().toUtf8());
                }
            }
            if (OpcUa_IsGood(monitoredItemCreateResults[count].StatusCode))
            {
                printf("** Event MonitoredItemId = %" OpcUa_PriUInt32 "\n", monitoredItemCreateResults[count].MonitoredItemId);
            }
            else
            {
                printf("** Event MonitoredItem for Server Object failed!\n");
            }
            printf("****************************************************************\n");
        }
    }

    printf("\n*******************************************************\n");
    printf("*******************************************************\n");
    printf("** We wait for 3 seconds then session is closed  ******\n");
    printf("*******************************************************\n");
    printf("*******************************************************\n");

    UaThread::sleep(3);

    OpcUa_UInt32 subscriptionId = pUaSubscription->subscriptionId();
    UaSubscription* pUaSubscription2 = NULL;

    bool doLoop = true;
    while (doLoop)
    {
        doLoop = false;

        // Disconnect from server but keep durable subcription
        g_pUaSession->disconnect(serviceSettings, OpcUa_False);
        // This emulates a disconnected client that comes back later to continue with data delivery


        printf("\n*******************************************************\n");
        printf("*******************************************************\n");
        printf("**        Press x to recreate session           *******\n");
        printf("**        and transfer durable subscription     *******\n");
        printf("*******************************************************\n");
        printf("*******************************************************\n");

        int action;
        /******************************************************************************/
        /* Wait for user command to stop the subscription. */
        while (!WaitForKeypress(action))
        {
            UaThread::msleep(100);
        }
        /******************************************************************************/


        SessionConnectInfo sessionConnectInfo;
        sessionConnectInfo.sApplicationName = g_pClientSampleConfig->sApplicationName();
        sessionConnectInfo.sApplicationUri = g_pClientSampleConfig->sApplicationUri();
        sessionConnectInfo.sProductUri = g_pClientSampleConfig->sProductUri();
        sessionConnectInfo.sSessionName = UaString("Client_Cpp_SDK@%1").arg(g_pClientSampleConfig->sHostName());
        sessionConnectInfo.bAutomaticReconnect = OpcUa_True;

        /*********************************************************************
        Connect to OPC UA Server
        **********************************************************************/
        status = g_pUaSession->connect(
            sUrl,                // URL of the Endpoint - from discovery or config
            sessionConnectInfo,  // General settings for connection
            sessionSecurityInfo, // Security settings
            g_pCallback);        // Callback interface
                                 /*********************************************************************/
        if (status.isBad())
        {
            delete g_pUaSession;
            g_pUaSession = NULL;
            printf("** Error: UaSession::connect failed [ret=%s]\n", status.toString().toUtf8());
            printf("****************************************************************\n");

            return;
        }

        UaUInt32Array   availableSequenceNumbers;

        status = g_pUaSession->transferSubscription(
            serviceSettings,
            g_pCallback,
            0,
            subscriptionId,
            subscriptionSettings,
            OpcUa_True,
            OpcUa_True,
            &pUaSubscription2,
            availableSequenceNumbers);
        if (status.isBad())
        {
            printf("** Error: UaSession::transferSubscription failed [ret=%s] **\n", status.toString().toUtf8());
        }
        else
        {
            printf("** OK: UaSession::transferSubscription succeeded **\n");
        }


        printf("\n*******************************************************\n");
        printf("*******************************************************\n");
        printf("**    Press x to delete durabel subscription          *\n");
        printf("**    Press 1 to continue disconnect / reconnect loop *\n");
        printf("*******************************************************\n");
        printf("*******************************************************\n");

        /******************************************************************************/
        /* Wait for user command to stop the subscription. */
        while (!WaitForKeypress(action))
        {
            if (action == 1)
            {
                doLoop = true;
                break;
            }
            UaThread::msleep(100);
        }
        /******************************************************************************/
    }

    /*********************************************************************
    Delete Subscription
    **********************************************************************/
    status = g_pUaSession->deleteSubscription(
        serviceSettings,    // Use default settings
        &pUaSubscription2);  // Subcription
                            /*********************************************************************/
    if (status.isBad())
    {
        printf("** UaSession::deleteSubscription failed! **********************\n");
        printf("****************************************************************\n");
    }
    else
    {
        pUaSubscription = NULL;
        printf("** UaSession::deleteSubscription succeeded!\n");
        printf("****************************************************************\n");
    }
}

callback.h

class Callback:
    public UaSessionCallback,
    public UaSubscriptionCallback
{
    /** 数据回调接口 Send changed data. */
    virtual void dataChange(
        OpcUa_UInt32               clientSubscriptionHandle,
        const UaDataNotifications& dataNotifications,
        const UaDiagnosticInfos&   /*diagnosticInfos*/)
    {
        OpcUa_UInt32 i = 0;
        printf("-- Event dataChange -----------------------------------\n");
        printf("clientSubscriptionHandle %d \n", clientSubscriptionHandle);
        for ( i=0; i<dataNotifications.length(); i++ )
        {
            if ( OpcUa_IsGood(dataNotifications[i].Value.StatusCode) )
            {
                UaVariant tempValue = dataNotifications[i].Value.Value;
                if (tempValue.type() == OpcUaType_ExtensionObject)
                {
                    printExtensionObjects(tempValue, UaString("Variable %1").arg(dataNotifications[i].ClientHandle));
                }
                else
                {
                    printf("  Variable %d value = %s\n", dataNotifications[i].ClientHandle, tempValue.toString().toUtf8());
                }
            }
            else
            {
                UaStatus statusError(dataNotifications[i].Value.StatusCode);
                printf("  Variable %d status = %s\n", i, statusError.toString().toUtf8());
            }
        }
        printf("-------------------------------------------------------\n");
    }

 /** 事件回调接口 Send new events. */
    virtual void newEvents(
        OpcUa_UInt32          clientSubscriptionHandle,
        UaEventFieldLists&    eventFieldList)
    {
        OpcUa_UInt32 i = 0;
        OpcUa_Int32 j = 0;
        printf("-- Event newEvents ------------------------------------\n");
        printf("clientSubscriptionHandle %d \n", clientSubscriptionHandle);
        for ( i=0; i<eventFieldList.length(); i++ )
        {
            // Print event message text first
            UaVariant message    = eventFieldList[i].EventFields[0];
            printf("Event[%d] Message = %s\n",
                i,
                message.toString().toUtf8());

            // Print other requested event fields in a list
            // Event field names must be set in m_eventFields before creating the event monitored item
            for ( j=1; j<eventFieldList[i].NoOfEventFields; j++ )
            {
                if ( (OpcUa_Int32)(m_eventFields.length() + 1) < j )
                {
                    printf("  More event fields delivered than requested\n");
                }
                UaVariant tempValue = eventFieldList[i].EventFields[j];
                if (tempValue.type() == OpcUaType_ExtensionObject)
                {
                    printExtensionObjects(tempValue, UaString(m_eventFields[j-1]));
                }
                else if (tempValue.type() == OpcUaType_Null)
                {
                    // Special handling for NULL values
                    printf("  %s = NULL\n", UaString(m_eventFields[j-1]).toUtf8());
                }
                else
                {
                    printf("  %s = %s\n", UaString(m_eventFields[j-1]).toUtf8(), tempValue.toString().toUtf8());
                }
            }
        }
        printf("-------------------------------------------------------\n");
    }
};