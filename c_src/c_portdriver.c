/* c_portdriver.c */

#include <stdio.h>
#include "erl_driver.h"
#include "complex.h"


typedef struct {
    // 端口句柄, 用于和Erlang进程通信
    ErlDrvPort port;
} example_data_t;

// 端口打开回调

static ErlDrvData c_portdriver_start(ErlDrvPort port, char *buff)
{
    // 给分 example_data_t 结构配内存
    example_data_t* d = (example_data_t*) driver_alloc(sizeof(example_data_t));
    // 设置端口
    d->port = port;
    // 返回数据结构
    return (ErlDrvData)d;
}

// 端口关闭回调

static void c_portdriver_stop(ErlDrvData handle)
{
    // 释放内存
    driver_free((char*)handle);
}

// 消息发送回调, 当Erlang 向 Port 发送消息时

static void c_portdriver_output(ErlDrvData handle, char *buff, ErlDrvSizeT bufflen)
{
    example_data_t* d = (example_data_t*)handle;
    char fn = buff[0], arg = buff[1], res;
    // 调用complex.c文件中的函数
    if (fn == 1) {
      res = foo(arg);
    } else if (fn == 2) {
      res = bar(arg);
    }
    // 把返回值发送给Erlang VM, 通过端口发送给在Erlang VM中的链接进程
    driver_output(d->port, &res, 1);
}


ErlDrvEntry example_driver_entry = {
    NULL,                           /* F_PTR init, called when driver is loaded */
    c_portdriver_start,             /* L_PTR start,  called when port is opened */
    c_portdriver_stop,              /* F_PTR stop,   called when port is closed */
    c_portdriver_output,            /* F_PTR output, called when erlang has sent */
    NULL,                           /* F_PTR ready_input,  called when input descriptor ready */
    NULL,                           /* F_PTR ready_output, called when output descriptor ready */
    "c_portdriver",                 /* char *driver_name, the argument to open_port */
    NULL,                           /* F_PTR finish, called when unloaded */
    NULL,                           /* void *handle, Reserved by VM */
    NULL,                           /* F_PTR control, port_command callback */
    NULL,                           /* F_PTR timeout, reserved */
    NULL,                           /* F_PTR outputv, reserved */
    NULL,                           /* F_PTR ready_async, only for async drivers */
    NULL,                           /* F_PTR flush, called when port is about
                                       to be closed, but there is data in driver
                                       queue */
    NULL,                           /* F_PTR call, much like control, sync call to driver */
    NULL,                           /* F_PTR event, called when an event selected
                                       by driver_event() occurs. */
    ERL_DRV_EXTENDED_MARKER,        /* int extended marker, Should always be
                                       set to indicate driver versioning */
    ERL_DRV_EXTENDED_MAJOR_VERSION, /* int major_version, should always be
                                      set to this value */
    ERL_DRV_EXTENDED_MINOR_VERSION, /* int minor_version, should always be
                                       set to this value */
    0,                              /* int driver_flags, see documentation */
    NULL,                           /* void *handle2, reserved for VM use */
    NULL,                           /* F_PTR process_exit, called when a
                                       monitored process dies */
    NULL                            /* F_PTR stop_select, called to close an
                                       event object */
};

// 驱动结构用驱动名称和函数指针填充, 它使用驱动结构, 并且包含头文件 erl_driver.h
DRIVER_INIT(c_portdriver) /* must match name in driver_entry */
{
    return &example_driver_entry;
}
