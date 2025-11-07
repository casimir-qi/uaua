
xml文档 初始和清理
函数


UaXmlDocument::initParser  
UaXmlDocument::cleanupParser


UaPlatformLayer::init
UaPlatformLayer::cleanup 

UaSession::connect
UaSession::disconnect  

UaSession::createSubscription
UaSession::deleteSubscription

UaSession::call

# 文档
## UaXmlDocument

void UaXmlDocument::initParser	(		)	
static
Initializes the XML parser.

None of the xml functions can be used before the XML parser is initialized.

void UaXmlDocument::cleanupParser	(		)	
static
Cleans up the XML parser.

None of the xml functions can be used after the XML parser is released.

## UaPlatformLayer
int UaPlatformLayer::init	(		)	
static
Initialize UA Stack platform layer.

None of the OpcUa_xxx functions can be used before the platform layer is assigned to the stack with this method.


void UaPlatformLayer::cleanup	(		)	
static
Clean up UA Stack platform layer.

None of the OpcUa_xxx functions can be used after calling this method.

## UaSession
文档来源：classUaClientSdk_1_1UaSession.html

The UaSession class manges a UA Client side application session.
### connect
UaStatus 	connect (const UaString &sURL, SessionConnectInfo &sessionConnectInfo, SessionSecurityInfo &sessionSecurityInfo, UaSessionCallback *pSessionCallback)
 	Establish a connection to the OPC UA server.
### disconnect
UaStatus 	disconnect (ServiceSettings &serviceSettings, OpcUa_Boolean bDeleteSubscriptions)
 	Disconnect client application from OPC UA server.

### createSubscription
UaStatus 	createSubscription (ServiceSettings &serviceSettings, UaSubscriptionCallback *pUaSubscriptionCallback, OpcUa_UInt32 clientSubscriptionHandle, SubscriptionSettings &subscriptionSettings, OpcUa_Boolean publishingEnabled, UaSubscription **ppUaSubscription)
 	Creates a subscription.

### deleteSubscription
UaStatus 	deleteSubscription (ServiceSettings &serviceSettings, UaSubscription **ppUaSubscription, OpcUa_UInt32 waitTimeForTransactionCompletion=500)
 	Deletes a subscription. 

### call
UaStatus 	call (ServiceSettings &serviceSettings, const CallIn &callRequest, CallOut &results)
 	Calls a method of an Object on the OPC UA server.
### read
UaStatus 	read (ServiceSettings &serviceSettings, OpcUa_Double maxAge, OpcUa_TimestampsToReturn timeStamps, const UaReadValueIds &nodesToRead, UaDataValues &values, UaDiagnosticInfos &diagnosticInfos)
 	Reads Attribute values synchronously from an OPC UA server. 
### write
UaStatus 	write (ServiceSettings &serviceSettings, const UaWriteValues &nodesToWrite, UaStatusCodeArray &results, UaDiagnosticInfos &diagnosticInfos)
 	Writes variable values synchronously to an OPC UA server. 



int main()
{
    UaXmlDocument::initParser  
    UaPlatformLayer::init


    UaSession::connect
    UaSession::read
    UaSession::write

    UaSession::createSubscription
    UaSession::deleteSubscription

    UaSession::call
    UaSession::disconnect  
    UaPlatformLayer::cleanup 
    UaXmlDocument::cleanupParser
}