#ifndef STECAMFOCUS_H
#define STECAMFOCUS_H

enum {
    ROI_LEFT,
    ROI_TOP,
    ROI_WIDTH,
    ROI_HEIGHT,
    ROI_REFERENCE_WIDTH,
    ROI_REFERENCE_HEIGHT,
    ROI_MAX
};

/* getROICoordinates */
inline bool getROICoordinates(char **str, int roi[], char delim=',');

#include "STECamFocus.inl"

#endif // STECAMFOCUS_H