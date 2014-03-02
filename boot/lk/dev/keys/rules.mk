LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/keys.o

ifeq ($(KEYS_USE_GPIO_KEYPAD),1)
OBJS += \
	$(LOCAL_DIR)/gpio_keypad.o
endif

ifeq ($(KEYS_USE_SSBI_PM8058_KEYPAD),1)
OBJS += \
	$(LOCAL_DIR)/ssbi_pm8058_keypad.o
endif

ifeq ($(KEYS_USE_UIB_KEYPAD),1)
OBJS += \
	$(LOCAL_DIR)/uib_keypad.o
endif
