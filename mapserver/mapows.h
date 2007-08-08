/**********************************************************************
 * $Id$
 *
 * mapows.h - OGC Web Services (WMS, WFS, WCS) support function definitions
 *
 **********************************************************************/

#ifndef MAPOWS_H
#define MAPOWS_H

#include <time.h>

/* This is the URL to the official OGC Schema Repository. We use it by 
 * default for OGC services unless the ows_schemas_lcoation web metadata 
 * is set in the mapfile.
 */
#define OWS_DEFAULT_SCHEMAS_LOCATION   "http://schemas.opengis.net"

/*====================================================================
 *   maphttp.c
 *====================================================================*/

#define MS_HTTP_SUCCESS(status)  (status == 200 || status == 242)

typedef struct http_request_info
{
    int     nLayerId;
    char    *pszGetUrl;
    char    *pszOutputFile;
    int     nTimeout;
    rectObj bbox;
    int     nStatus;            /* 200=success, value < 0 if request failed */
    char    *pszContentType;    /* Content-Type of the response */
    char    *pszErrBuf;         /* Buffer where curl can write errors */
    char    *pszPostRequest;    /* post request content (NULL for GET) */
    char    *pszPostContentType;/* post request MIME type */
    char    *pszUserAgent;      /* User-Agent, auto-generated if not set */

    /* For debugging/profiling */
    int         debug;         /* Debug mode?  MS_TRUE/MS_FALSE */

    /* Private members */
    void      * curl_handle;   /* CURLM * handle */
    FILE      * fp;            /* FILE * used during download */
} httpRequestObj;

typedef  struct
{
  char *pszVersion;
  char *pszRequest;
  char *pszService;
  char *pszTypeName;
  char *pszFilter;
  int nMaxFeatures;
  char *pszBbox; /* only used with a Get Request */
  char *pszOutputFormat; /* only used with DescibeFeatureType */
  char *pszFeatureId;

} wfsParamsObj;


/* wmsParamsObj
 *
 * Used to preprocess WMS request parameters and combine layers that can
 * be comined in a GetMap request.
 */
typedef  struct
{
  char        *onlineresource;
  hashTableObj *params;
  int          numparams;
} wmsParamsObj;

int msHTTPInit(void);
void msHTTPCleanup(void);

void msHTTPInitRequestObj(httpRequestObj *pasReqInfo, int numRequests);
void msHTTPFreeRequestObj(httpRequestObj *pasReqInfo, int numRequests);
int  msHTTPExecuteRequests(httpRequestObj *pasReqInfo, int numRequests,
                           int bCheckLocalCache);
int  msHTTPGetFile(const char *pszGetUrl, const char *pszOutputFile, 
                   int *pnHTTPStatus, int nTimeout, int bCheckLocalCache,
                   int bDebug);


/*====================================================================
 *   mapows.c
 *====================================================================*/
MS_DLL_EXPORT int msOWSDispatch(mapObj *map, cgiRequestObj *request);

#if defined(USE_WMS_SVR) || defined (USE_WFS_SVR) || defined (USE_WCS_SVR) || defined(USE_SOS_SVR)

MS_DLL_EXPORT int msOWSMakeAllLayersUnique(mapObj *map);
MS_DLL_EXPORT char *msOWSTerminateOnlineResource(const char *src_url);
MS_DLL_EXPORT char *msOWSGetOnlineResource(mapObj *map, const char *namespaces, const char *metadata_name, cgiRequestObj *req);
MS_DLL_EXPORT const char *msOWSGetSchemasLocation(mapObj *map);
MS_DLL_EXPORT const char *msOWSGetLanguage(mapObj *map, const char *context);

/* Constants for OWS Service version numbers */
#define OWS_0_1_2   0x000102
#define OWS_0_1_4   0x000104
#define OWS_0_1_6   0x000106
#define OWS_0_1_7   0x000107
#define OWS_1_0_0   0x010000
#define OWS_1_0_6   0x010006
#define OWS_1_0_7   0x010007
#define OWS_1_0_8   0x010008
#define OWS_1_1_0   0x010100
#define OWS_1_1_1   0x010101
#define OWS_VERSION_MAXLEN   20  /* Buffer size for msOWSGetVersionString() */

MS_DLL_EXPORT int msOWSParseVersionString(const char *pszVersion);
MS_DLL_EXPORT const char *msOWSGetVersionString(int nVersion, char *pszBuffer);


/* OWS_NOERR and OWS_WARN passed as action_if_not_found to printMetadata() */
#define OWS_NOERR   0
#define OWS_WARN    1

/* OWS_WMS and OWS_WFS used for functions that differ in behavior between */
/* WMS and WFS services (e.g. msOWSPrintLatLonBoundingBox()) */
#define OWS_WMS     1
#define OWS_WFS     2

MS_DLL_EXPORT const char * msOWSLookupMetadata(hashTableObj *metadata, 
                                    const char *namespaces, const char *name);
MS_DLL_EXPORT int msOWSPrintMetadata(FILE *stream, hashTableObj *metadata, 
                       const char *namespaces, const char *name, 
                       int action_if_not_found, const char *format, 
                       const char *default_value);
int msOWSPrintEncodeMetadata(FILE *stream, hashTableObj *metadata, 
                             const char *namespaces, const char *name, 
                             int action_if_not_found, 
                             const char *format, const char *default_value) ;
char *msOWSGetEncodeMetadata(hashTableObj *metadata, 
                             const char *namespaces, const char *name, 
                             const char *default_value);

int msOWSPrintValidateMetadata(FILE *stream, hashTableObj *metadata, 
                               const char *namespaces, const char *name, 
                               int action_if_not_found, 
                               const char *format, const char *default_value);
int msOWSPrintGroupMetadata(FILE *stream, mapObj *map, char* pszGroupName, 
                            const char *namespaces, const char *name, 
                            int action_if_not_found, 
                            const char *format, const char *default_value);
int msOWSPrintURLType(FILE *stream, hashTableObj *metadata, 
                      const char *namespaces, const char *name, 
                      int action_if_not_found, const char *tag_format, 
                      const char *tag_name, const char *type_format, 
                      const char *width_format, const char *height_format, 
                      const char *urlfrmt_format, const char *href_format,
                      int type_is_mandatory, int width_is_mandatory, 
                      int height_is_mandatory, int format_is_mandatory, 
                      int href_is_mandatory, const char *default_type, 
                      const char *default_width, const char *default_height, 
                      const char *default_urlfrmt, const char *default_href, 
                      const char *tabspace);
int msOWSPrintParam(FILE *stream, const char *name, const char *value, 
                    int action_if_not_found, const char *format, 
                    const char *default_value);
int msOWSPrintEncodeParam(FILE *stream, const char *name, const char *value, 
                          int action_if_not_found, const char *format, 
                          const char *default_value);
int msOWSPrintMetadataList(FILE *stream, hashTableObj *metadata, 
                           const char *namespaces, const char *name, 
                           const char *startTag, 
                           const char *endTag, const char *itemFormat,
                           const char *default_value);
int msOWSPrintEncodeMetadataList(FILE *stream, hashTableObj *metadata, 
                                 const char *namespaces, const char *name, 
                                 const char *startTag, 
                                 const char *endTag, const char *itemFormat,
                                 const char *default_value);
int msOWSPrintEncodeParamList(FILE *stream, const char *name, 
                              const char *value, int action_if_not_found, 
                              char delimiter, const char *startTag, 
                              const char *endTag, const char *format, 
                              const char *default_value);
void msOWSPrintLatLonBoundingBox(FILE *stream, const char *tabspace, 
                                 rectObj *extent, projectionObj *srcproj,
                                 int nService);
void msOWSPrintBoundingBox(FILE *stream, const char *tabspace, 
                           rectObj *extent, 
                           projectionObj *srcproj,
                           hashTableObj *metadata,
                           const char *namespaces);
void msOWSPrintContactInfo( FILE *stream, const char *tabspace, 
                            int nVersion, hashTableObj *metadata,
                            const char *namespaces  );
int msOWSGetLayerExtent(mapObj *map, layerObj *lp, const char *namespaces, rectObj *ext);
int msOWSExecuteRequests(httpRequestObj *pasReqInfo, int numRequests,
                         mapObj *map, int bCheckLocalCache);
void msOWSProcessException(layerObj *lp, const char *pszFname, 
                           int nErrorCode, const char *pszFuncName);
char *msOWSBuildURLFilename(const char *pszPath, const char *pszURL, 
                            const char *pszExt);
const char *msOWSGetEPSGProj(projectionObj *proj, hashTableObj *metadata, const char *namespaces, int bReturnOnlyFirstOne);

void msOWSGetDimensionInfo(layerObj *layer, const char *pszDimension, 
                           const char **pszDimUserValue, 
                           const char **pszDimUnits, 
                           const char **pszDimDefault, 
                           const char **pszDimNearValue, 
                           const char **pszDimUnitSymbol, 
                           const char **pszDimMultiValue);

#endif

/*====================================================================
 *   mapgml.c
 *====================================================================*/
#define OWS_GML2 0 /* Supported GML formats */
#define OWS_GML3 1

#define OWS_WFS_FEATURE_COLLECTION_NAME "msFeatureCollection"
#define OWS_GML_DEFAULT_GEOMETRY_NAME "msGeometry"
#define OWS_GML_OCCUR_UNBOUNDED -1

#if defined(USE_WMS_SVR) || defined (USE_WFS_SVR)

/* TODO, there must be a better way to generalize these lists of objects... */

typedef struct {
  char *name;     /* name of the item */
  char *alias;    /* is the item aliased for presentation? (NULL if not) */
  char *type;     /* raw type for this item (NULL for a "string") (TODO: should this be a lookup table instead?) */
#ifndef __cplusplus 
  char *template;  /* presentation string for this item, needs to be a complete XML tag */
#else
  char *_template;  /* presentation string for this item, needs to be a complete XML tag */
#endif
  int encode;     /* should the value be HTML encoded? Default is MS_TRUE */
  int visible;    /* should this item be output, default is MS_FALSE */  
} gmlItemObj;

typedef struct {
  gmlItemObj *items;
  int numitems;
} gmlItemListObj;

typedef struct {
  char *name;     /* name of the constant */
  char *type;     /* raw type for this item (NULL for a "string") */
  char *value;    /* output value for this constant (output will look like: <name>value</name>) */
} gmlConstantObj;

typedef struct {
  gmlConstantObj *constants;
  int numconstants;
} gmlConstantListObj;

typedef struct {
  char *name;     /* name of the geometry (type of GML property) */
  char *type;     /* raw type for these geometries (point|multipoint|line|multiline|polygon|multipolygon */
  int occurmin, occurmax;   /* number of occurances (default 0,1) */
} gmlGeometryObj;

typedef struct {
  gmlGeometryObj *geometries;
  int numgeometries;
} gmlGeometryListObj;

typedef struct {
  char *name;     /* name of the group */
  char **items;   /* list of items in the group */
  int numitems;   /* number of items */
  char *type;     /* name of the complex type */
} gmlGroupObj;

typedef struct {
  gmlGroupObj *groups;
  int numgroups;
} gmlGroupListObj;

typedef struct {
	char *prefix;
	char *uri;
	char *schemalocation;
} gmlNamespaceObj;

typedef struct {
  gmlNamespaceObj *namespaces;
  int numnamespaces;
} gmlNamespaceListObj;

MS_DLL_EXPORT int msItemInGroups(char *name, gmlGroupListObj *groupList);
MS_DLL_EXPORT gmlItemListObj *msGMLGetItems(layerObj *layer, const char *metadata_namespaces);
MS_DLL_EXPORT void msGMLFreeItems(gmlItemListObj *itemList);
MS_DLL_EXPORT gmlConstantListObj *msGMLGetConstants(layerObj *layer, const char *metadata_namespaces);
MS_DLL_EXPORT void msGMLFreeConstants(gmlConstantListObj *constantList);
MS_DLL_EXPORT gmlGeometryListObj *msGMLGetGeometries(layerObj *layer, const char *metadata_namespaces);
MS_DLL_EXPORT void msGMLFreeGeometries(gmlGeometryListObj *geometryList);
MS_DLL_EXPORT gmlGroupListObj *msGMLGetGroups(layerObj *layer, const char *metadata_namespaces);
MS_DLL_EXPORT void msGMLFreeGroups(gmlGroupListObj *groupList);
MS_DLL_EXPORT gmlNamespaceListObj *msGMLGetNamespaces(webObj *web, const char *metadata_namespaces);
MS_DLL_EXPORT void msGMLFreeNamespaces(gmlNamespaceListObj *namespaceList);
#endif

/* export to fix bug 851 */
MS_DLL_EXPORT int msGMLWriteQuery(mapObj *map, char *filename, const char *namespaces);


#ifdef USE_WFS_SVR
MS_DLL_EXPORT int msGMLWriteWFSQuery(mapObj *map, FILE *stream, int maxfeatures, char *wfs_namespace, int outputformat);
#endif


/*====================================================================
 *   mapwms.c
 *====================================================================*/
int msWMSDispatch(mapObj *map, cgiRequestObj *req); 
MS_DLL_EXPORT int msWMSLoadGetMapParams(mapObj *map, int nVersion,
                          char **names, char **values, int numentries);


/*====================================================================
 *   mapwmslayer.c
 *====================================================================*/

int msInitWmsParamsObj(wmsParamsObj *wmsparams);
void msFreeWmsParamsObj(wmsParamsObj *wmsparams);

int msPrepareWMSLayerRequest(int nLayerId, mapObj *map, layerObj *lp,
                             enum MS_CONNECTION_TYPE lastconnectiontype,
                             wmsParamsObj *psLastWMSParams,
                             httpRequestObj *pasReqInfo, int *numRequests);
int msDrawWMSLayerLow(int nLayerId, httpRequestObj *pasReqInfo, 
                      int numRequests, mapObj *map, layerObj *lp, 
                      imageObj *img);
MS_DLL_EXPORT char *msWMSGetFeatureInfoURL(mapObj *map, layerObj *lp,
                             int nClickX, int nClickY, int nFeatureCount,
                             const char *pszInfoFormat); 


/*====================================================================
 *   mapwfs.c
 *====================================================================*/

/* Supported DescribeFeature formats */
#define OWS_DEFAULT_SCHEMA 0 /* basically a GML 2.1 schema */
#define OWS_SFE_SCHEMA 1 /* GML for simple feature exchange (formerly GML3L0) */

int msWFSDispatch(mapObj *map, cgiRequestObj *requestobj);
void msWFSParseRequest(cgiRequestObj *, wfsParamsObj *);
wfsParamsObj *msWFSCreateParamsObj(void);
void msWFSFreeParamsObj(wfsParamsObj *wfsparams);

#ifdef USE_WFS_SVR
const char *msWFSGetGeomElementName(mapObj *map, layerObj *lp);
#endif


/*====================================================================
 *   mapwfslayer.c
 *====================================================================*/

int msPrepareWFSLayerRequest(int nLayerId, mapObj *map, layerObj *lp,
                             httpRequestObj *pasReqInfo, int *numRequests);
void msWFSUpdateRequestInfo(layerObj *lp, httpRequestObj *pasReqInfo);
int msWFSLayerOpen(layerObj *lp, 
                   const char *pszGMLFilename, rectObj *defaultBBOX);
int msWFSLayerIsOpen(layerObj *lp); 
int msWFSLayerInitItemInfo(layerObj *layer);
int msWFSLayerGetItems(layerObj *layer);
int msWFSLayerWhichShapes(layerObj *layer, rectObj rect);
int msWFSLayerClose(layerObj *lp);
MS_DLL_EXPORT char *msWFSExecuteGetFeature(layerObj *lp);

/*====================================================================
 *   mapcontext.c
 *====================================================================*/

MS_DLL_EXPORT int msWriteMapContext(mapObj *map, FILE *stream);
MS_DLL_EXPORT int msSaveMapContext(mapObj *map, char *filename);
MS_DLL_EXPORT int msLoadMapContext(mapObj *map, char *filename, int unique_layer_names);
MS_DLL_EXPORT int msLoadMapContextURL(mapObj *map, char *urlfilename, int unique_layer_names);


/*====================================================================
 *   mapwcs.c
 *====================================================================*/

int msWCSDispatch(mapObj *map, cgiRequestObj *requestobj); /* only 1 public function */



/*====================================================================
 *   mapogsos.c
 *====================================================================*/

int msSOSDispatch(mapObj *map, cgiRequestObj *requestobj); /* only 1 public function */


#endif /* MAPOWS_H */
