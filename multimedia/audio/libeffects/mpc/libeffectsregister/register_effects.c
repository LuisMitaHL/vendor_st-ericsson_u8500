#include <libeffects/mpc/libeffectsregister.nmf>
#include <dbc.h>

/// maximum number of effect
#define MAXREGISTERED 16

typedef struct registryEffect_t {
    char sName[EFFECT_NAME_MAX_LENGTH+1];       //Name of the registered effect
    EFFECT_FACTORYMETHODTYPE pFactoryMethod;    //Factory Method of the effect
} registryEffect_t;

int mRegisteredCount;
registryEffect_t EXTMEM mEffect[MAXREGISTERED];

///////////////////////////////////////////////////////////////////////////////////

static int strlen(const char *string)
{
    int size = 0;
    while (*string++ != '\0') {
        size++;
    }
    return size;
}

static int strcmp(const char *string1, const char *string2)
{
    while ((*string1++ == *string2++) && (*string1 != '\0'));
    return (*(unsigned char *)--string1 - *(unsigned char *)--string2);
}

static char* strcpy(char * dest, const char * src) {
    int i = 0;
    while(src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    return dest;
}

static void lookupEffect(const char *sEffectName, EFFECT_FACTORYMETHODTYPE *ppFactoryMethod)
{
    int i = 0;

    ASSERT(sEffectName!=NULL);
    ASSERT(strlen(sEffectName) <= EFFECT_NAME_MAX_LENGTH);

    for (i = 0; i < mRegisteredCount; i++) {
        if (!strcmp(sEffectName, &mEffect[i].sName)) {
            *ppFactoryMethod = mEffect[i].pFactoryMethod;
            return;
        }
    }

    *ppFactoryMethod = NULL;
}

///////////////////////////////////////////////////////////////////////////////////

void registerEffect(const char *sEffectName, EFFECT_FACTORYMETHODTYPE pFactoryMethod)
{
    EFFECT_FACTORYMETHODTYPE pDummyFactoryMethod = NULL;

    ASSERT(sEffectName!=NULL && pFactoryMethod!=NULL);
    ASSERT(mRegisteredCount < MAXREGISTERED);

    lookupEffect(sEffectName, &pDummyFactoryMethod);
    ASSERT(pDummyFactoryMethod==NULL);

    strcpy(&(mEffect[mRegisteredCount].sName), sEffectName);
    mEffect[mRegisteredCount].pFactoryMethod = pFactoryMethod;
    mRegisteredCount ++;
}

EffectDescription_t * createEffect(const char *sEffectName)
{
    EFFECT_FACTORYMETHODTYPE pFactoryMethod = NULL;
    EffectDescription_t *pEffectDescription = NULL;

    lookupEffect(sEffectName, &pFactoryMethod);
    ASSERT(pFactoryMethod!=NULL);

    pEffectDescription = pFactoryMethod();
    ASSERT(pEffectDescription!=NULL);

    return pEffectDescription;
}

///////////////////////////////////////////////////////////////////////////////////

t_nmf_error METH(construct)(void)
{
    int i = 0;

    mRegisteredCount = 0;

    for (i = 0; i < MAXREGISTERED; i++) {
        mEffect[i].sName[0] = '\0';
        mEffect[i].pFactoryMethod = NULL;
    }

	return NMF_OK;
}

