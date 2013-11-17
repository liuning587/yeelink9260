/**
 ******************************************************************************
 * @file      main.c
 * @brief     C Source file of main.c.
 * @details   This file including all API functions's 
 *            implement of main.c.	
 *
 * @copyright
 ******************************************************************************
 */
 
/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "config.h"
#include "socket.h"
#include "log.h"

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Constant Definitions
 ----------------------------------------------------------------------------*/
/* NONE */ 

/*-----------------------------------------------------------------------------
 Section: Global Variables
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Local Variables
 ----------------------------------------------------------------------------*/
/**
 * 一下字符串有四个参数
 * 1：设备号
 * 2：传感器号
 * 3: key密码
 * 4: json长度
 * 5: json数据体
 */
static const char *pDataPoint = "POST /v1.0/device/%s/sensor/%s/datapoints HTTP/1.1\r\n"
        "Host: api.yeelink.net\r\n"
        "Accept: */*\r\n"
        "U-ApiKey: %s\r\n"
        "Content-Length: %d\r\n"
        "Content-type: application/json;charset=utf-8\r\n"
        "Connection: close\r\n\r\n"
        "%s\r\n";

/*-----------------------------------------------------------------------------
 Section: Local Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Global Function Prototypes
 ----------------------------------------------------------------------------*/
/* NONE */

/*-----------------------------------------------------------------------------
 Section: Function Definitions
 ----------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @brief   将数据发送至yeelink
 * @param[in]  device   : 设备编号
 * @param[in]  sensor   : 传感器编号
 * @param[in]  *key     : key
 * @param[in]  t        : 数据时标
 * @param[in]  v        : 数据点的值
 *
 * @retval  -1 : ERROR
 * @retval   0 : OK
 ******************************************************************************
 */
const char *
send_data_to_yeelink(const char *device,
        const char *sensor,
        const char *key,
        time_t t,
        float v)
{
    static char sbuf[512];
    char json[64];
    struct tm curtm;
    int jsonlen = 0;

    t = (t == 0) ? time(NULL) : t;
    (void)localtime_r(&t, &curtm);
    (void)sprintf(json, "{\"timestamp\":\"%04d-%02d-%02dT%02d:%02d:%02d\","
            "\"value\":%.2f}",
            1900 + curtm.tm_year, curtm.tm_mon + 1, curtm.tm_mday, curtm.tm_hour, curtm.tm_min, curtm.tm_sec,
            v + 0.005f);
    jsonlen = strlen(json);

    sprintf(sbuf, pDataPoint, device, sensor, key, jsonlen, json);
    return sbuf;
}

/**
 ******************************************************************************
 * @brief   主函数
 * @param[in]  None
 * @param[out] None
 * @retval     None
 ******************************************************************************
 */
int main(int argc, char** argv)
{
    time_t pre_time = time(NULL);
    time_t cur_time = time(NULL);
    int socket = -1;
    float temp = 0.0f;
    float humidity = 0.0f;
    int count = 0;
    const char *psenbuf;
//    char buf[1024];
    int dht11_fd = 0;
    unsigned char datbuf[5];


    //1. 打开设备
    dht11_fd = open("/dev/dht11", 0);
    if (dht11_fd < 0)
    {
        perror("open device dht11");
        exit(1);
    }

    //2. 循环每隔1分钟上传1次数据
    while (1)
    {
        cur_time = time(NULL);

        //读取温度湿度
        memset(datbuf, 0x00, 5);
        if (5 != read(dht11_fd, datbuf, 5))
        {
            printf("read dht11 err!\n");
            continue;
        }
        humidity += datbuf[0] + datbuf[1] / 256.0f;
        temp += datbuf[2] + datbuf[3] / 256.0f;
        count++;

        if (abs((cur_time / 60) - (pre_time / 60)) >= DATA_MIN)
        {
            pre_time = cur_time;

            humidity /= count;
            temp /= count;

            printf("平均湿度:%.2f\n", temp);
            printf("平均温度:%.2f\n", humidity);

            socket = socket_init(HOST_NAME, PORT, 10 * 1000);
            if (socket <= 0)
            {
                printf("Can not connect to server!\n");
            }
            else
            {
                printf("send data!\n");
                /* 发送温度 */
                psenbuf = send_data_to_yeelink(DEVICE_NUM, SENSOR_TEMP, API_KEY, 0, temp);
//                printf("send:%s\n", psenbuf);
                if (0 >= socket_send(socket, psenbuf, strlen(psenbuf)))
                {
                    printf("Socket send error!\n");
                }
                else
                {
                    printf("send ok!\n");
                }
#if 0
                //todo: 这里是否需要查看服务器返回数据
                if (0 < socket_recv(socket, buf, sizeof(buf)))
                {
//                    printf("recv:%s\n", buf);
                }
                else
                {
                    printf("recv err!\n");
                }
#endif
                socket_close(socket);   /* 关闭socket连接 */
                printf("close!\n");
            }

            socket = socket_init(HOST_NAME, PORT, 10 * 1000);
            if (socket <= 0)
            {
                printf("Can not connect to server!\n");
            }
            else
            {
                printf("send data!\n");
                /* 发送湿度 */
                psenbuf = send_data_to_yeelink(DEVICE_NUM, SENSOR_HUMIDITY, API_KEY, 0, humidity);
//                printf("send:%s\n", psenbuf);
                if (0 >= socket_send(socket, psenbuf, strlen(psenbuf)))
                {
                    printf("Socket send error!\n");
                }
#if 0
                //todo: 这里是否需要查看服务器返回数据
                if (0 < socket_recv(socket, buf, sizeof(buf)))
                {
//                    printf("recv:%s\n", buf);
                }
                else
                {
                    printf("recv err!\n");
                }
#endif
                socket_close(socket);   /* 关闭socket连接 */
                printf("close!\n");
            }

            humidity = 0.0f;
            temp = 0.0f;
            count = 0;
        }
        sleep(1);
    }

    return 0;
}

/*-------------------------------main.c--------------------------------------*/
