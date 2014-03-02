#include "NmfComponent.h"

AFM_API_EXPORT t_cm_instance_handle & NmfComponent::getNmfMpcHandle(void) const{
    return (t_cm_instance_handle &)mNmfMain;
}

