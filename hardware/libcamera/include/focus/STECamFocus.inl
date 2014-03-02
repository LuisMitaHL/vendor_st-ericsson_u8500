#ifndef STECAMFOCUS_INL
#define STECAMFOCUS_INL

/* Return True on success, false on failure
   This function is valid for ROI_MAX >=2 */
inline bool getROICoordinates(char **str, int roi_coord[], char delim /*= ','*/)
{
    DBGT_PROLOG("String: %s", *str);

	if((NULL == str ) || (NULL == *str ) ) {
		DBGT_CRITICAL("OMX_SetConfig failed");
		DBGT_EPILOG("");
		return false;
	}
    char *StartPtr = (char *)*str;
    char *end;
    roi_coord[ROI_LEFT] = (int)strtol(StartPtr, &end, 10);
    DBGT_ASSERT(*end == delim, "Cannot find delimeter(%c) in string(%s)", delim, StartPtr);

    for( int i=ROI_TOP; i<(ROI_MAX-1);i++){
        roi_coord[i] = (int)strtol(end+1, &end, 10);
        DBGT_ASSERT(*end == delim, "Cannot find delimeter(%c) in string(%s)", delim, StartPtr);

    }
    roi_coord[ROI_MAX-1] = (int)strtol(end+1, &end, 10);
    *str = end;
	DBGT_EPILOG("");
	return true;}


#endif // STECAMFOCUS_INL