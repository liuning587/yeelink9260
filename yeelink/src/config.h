/**
 ******************************************************************************
 * @file       config.h
 * @brief      �����ļ�
 * @details    This file including all API functions's declare of config.h.
 * @copyright
 *
 ******************************************************************************
 */
#ifndef CONFIG_H_
#define CONFIG_H_ 

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define DEVICE_NUM              "4208"                  /**< �豸��� */
#define SENSOR_TEMP             "9405"                  /**< �¶ȴ�������� */
#define SENSOR_HUMIDITY         "9406"                  /**< ʪ�ȴ�������� */
#define API_KEY                 "0464e914baafd4d1a0439a10bcd0d000" /**< API-KEY */
#define DATA_MIN                1                       /**< �ϴ���������min */

#define HOST_NAME               "api.yeelink.net"
#define PORT                    80

#define DEFAULT_INI_FILE        "./RouterDial.ini"      /**< Ĭ�������ļ� */
#define DEFAULT_LOG_FILE_NAME   "./RouterDial.txt"      /**< Ĭ����־�ļ� */
#define DEFAULT_LOG_ON          (1u)                    /**< Ĭ�ϴ���־��¼ */

#endif /* CONFIG_H_ */
/*-----------------------------End of config.h-------------------------------*/
