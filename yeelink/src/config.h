/**
 ******************************************************************************
 * @file       config.h
 * @brief      配置文件
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
#define DEVICE_NUM              "4208"                  /**< 设备编号 */
#define SENSOR_TEMP             "9405"                  /**< 温度传感器编号 */
#define SENSOR_HUMIDITY         "9406"                  /**< 湿度传感器编号 */
#define API_KEY                 "0464e914baafd4d1a0439a10bcd0d000" /**< API-KEY */
#define DATA_MIN                1                       /**< 上传数据周期min */

#define HOST_NAME               "api.yeelink.net"
#define PORT                    80

#define DEFAULT_INI_FILE        "./RouterDial.ini"      /**< 默认配置文件 */
#define DEFAULT_LOG_FILE_NAME   "./RouterDial.txt"      /**< 默认日志文件 */
#define DEFAULT_LOG_ON          (1u)                    /**< 默认打开日志记录 */

#endif /* CONFIG_H_ */
/*-----------------------------End of config.h-------------------------------*/
