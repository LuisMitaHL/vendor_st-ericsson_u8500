include ../make.ost_conf

include $(MM_MAKEFILES_DIR)/SharedCheck.mk

#Do not run trace compiler but ... perform top dictionary generation
OST_TC_BYPASS=1

# Fill up OST_TOP_DICO_LIST variable for top dictionary generation
OST_TOP_DICO_LIST = $(OST_TOP_DICO_NAME_1)::$(OST_COMPONENT_ID_TOP_1)
OST_TOP_DICO_LIST += $(OST_TOP_DICO_NAME_2)::$(OST_COMPONENT_ID_TOP_2)

include $(MM_MAKEFILES_DIR)/SharedRules.mk

