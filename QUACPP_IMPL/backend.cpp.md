Open62541AsyncBackend

```
    // Callbacks
    static void asyncMethodCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncTranslateBrowsePathCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncAddNodeCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncDeleteNodeCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncAddReferenceCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncDeleteReferenceCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncReadCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncWriteAttributesCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncBrowseCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncBatchReadCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncBatchWriteCallback(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);
    static void asyncReadHistoryDataCallBack(UA_Client *client, void *userdata, UA_UInt32 requestId, void *response);

```