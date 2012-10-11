#include <windows.h>

#include "npapi.h"
#include "nptypes.h"
#include "npruntime.h"
#include "npfunctions.h"

#include "alert.h"

static NPObject *so;
static NPNetscapeFuncs *funcs;

static int mb_get_len(NPString src){
    return MultiByteToWideChar(CP_UTF8, 0, src.UTF8Characters,
                               src.UTF8Length, NULL, 0) + 1;
}
static int mb_to_wchar(NPString src, wchar_t *buf, int mb_len){
    int ret = MultiByteToWideChar(CP_UTF8, 0, src.UTF8Characters,
                                  src.UTF8Length, buf, mb_len);
    *(buf + mb_len - 1) = '\0';
    return ret;
}

static bool has_method(NPObject* obj, NPIdentifier methodName)
{
    return true;
	char *name = funcs->utf8fromidentifier(methodName);
	if ( strcmp(name, FUNCTION_MYFUNC) == 0 ) {
		return true;
	} else {
		return false;
	}
}
static bool invoke_default(NPObject *obj, const NPVariant *args,
                           uint32_t argCount, NPVariant *result)
{
	return true;
}

static bool invoke(NPObject* obj, NPIdentifier method_name,
                   const NPVariant *args, uint32_t arg_count, NPVariant *result)
{
	NPUTF8 *name = funcs->utf8fromidentifier(method_name);
    if (!name)
        return false;
	if(!strcmp((const char *)name, FUNCTION_MYFUNC)) {
        if(arg_count != 1){
            funcs->setexception(obj, "too many arguments!");
            return false;
        }
        NPString arg0 = NPVARIANT_TO_STRING(args[0]);

        int	len = mb_get_len(arg0);
        wchar_t *buf = new wchar_t[len];
        mb_to_wchar(arg0, buf, len);

        MessageBoxW(NULL, buf, TEXT("メッセージボックス"), MB_OK);
        delete buf;

		BOOLEAN_TO_NPVARIANT(true, *result);
		return true;
	} else {
		// aim exception handling
		funcs->setexception(obj, "exception during invocation");
		return false;
	}
}
static bool has_property(NPObject *obj, NPIdentifier propertyName)
{
	return false;
}

static bool get_property(NPObject *obj, NPIdentifier propertyName,
                         NPVariant *result)
{
	return false;
}

static NPClass npcRefObject = {
	NP_CLASS_STRUCT_VERSION,
	NULL,
	NULL,
	NULL,
	has_method,
	invoke,
	invoke_default,
	has_property,
	get_property,
	NULL,
	NULL,
    NULL,
    NULL
};

static NPError npp_new_proc_ptr(NPMIMEType pluginType, NPP instance,
                                uint16_t mode, int16_t argc, char *argn[],
                                char *argv[], NPSavedData *saved)
{
	return NPERR_NO_ERROR;
}

static NPError npp_destroy(NPP instance, NPSavedData **save)
{
    if (so)
        funcs->releaseobject(so);
    so = NULL;
    return NPERR_NO_ERROR;
}

static NPError npp_get_value(NPP instance, NPPVariable variable, void *value)
{
    switch (variable){
    case NPPVpluginNameString:
        DEBUGBOX(TEXT("VpluginNameString"));
        *((const char **)value) = "Ext Test";
        break;
    case NPPVpluginDescriptionString:
        DEBUGBOX(TEXT("VpluginDesc"));
        *((const char **)value) = "Chrome Extension Test Program";
        break;
    case NPPVpluginScriptableNPObject:
        DEBUGBOX(TEXT("ScriptableObj"));
        if (!so)
            so = funcs->createobject(instance, &npcRefObject);
        funcs->retainobject(so);
        *(NPObject **)value = so;
        break;
    case NPPVpluginNeedsXEmbed:
        DEBUGBOX(TEXT("needsXEmbed"));
        *((char *)value) = 1;
        break;
    default:
        return NPERR_GENERIC_ERROR;
    }
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs *nppfuncs)
{
    nppfuncs->version = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
    nppfuncs->size = sizeof(nppfuncs);
    nppfuncs->newp = npp_new_proc_ptr;
    nppfuncs->destroy = npp_destroy;
    nppfuncs->getvalue = npp_get_value;

    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs *npnf)
{
    DEBUGBOX(TEXT("Initialize"));
    if (npnf == NULL)
        return NPERR_INVALID_FUNCTABLE_ERROR;

    funcs = npnf;

    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown(void)
{
    return NPERR_NO_ERROR;
}

char *NP_GetMIMEDescription(void)
{
    return (char *)"application/x-alert-extension";
}
const char *NP_GetPluginVersion(void)
{
    return "1.0.0.1";
}

/* needs to be present for WebKit based browsers */
NPError OSCALL NP_GetValue(void *npp, NPPVariable variable, void *value)
{
    return npp_get_value((NPP)npp, variable, value);
}
