/*
接线说明:该程序应该烧录进源地工作室的板子中去

程序说明:该板子是客户端,根据板子上BOOT按键是否按下发送不同的HTTP信息,进而控制服务器端的LED灯的亮灭

注意事项:注意你的HTTP请求的书写格式,该程序曾经少打了一个空格导致服务器无法运行,客户端的串口输出正常,额,好难找到的错误

函数示例:无

作者:灵首

时间:2023_3_20

*/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>

WiFiMulti wifi_multi;  //建立WiFiMulti 的对象,对象名称是 wifi_multi

#define BOOTPin  0  //GPIO0 就是BOOT按键

static float clientFloatValue;
static int clientIntValue;
static int BOOTPinState;

//定义的函数

void wifi_multi_con(void);
void wifi_multi_init(void);

void wifiClientRequest(const char* host,const int httpPort);



/*
# brief 连接WiFi的函数
# param 无
# retval 无
*/
void wifi_multi_con(void){
  int i=0;
  while(wifi_multi.run() != WL_CONNECTED){
    delay(1000);
    i++;
    Serial.print(i);
  }

}


/*
# brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
# param 无
# retval  无
*/
void wifi_multi_init(void){
  wifi_multi.addAP("LINGSOU123","12345678");
  wifi_multi.addAP("LINGSOU12","12345678");
  wifi_multi.addAP("LINGSOU1","12345678");
  wifi_multi.addAP("LINGSOU234","12345678");   //通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}


/*
# brief 通过WiFiClient库向指定网址建立连接并发出信息
# param  const char* host:需要建立连接的网站的网址
# param   const int httpPort:对应的端口号
# retval  无,但是会通过串口打印一些内容
*/
void wifiClientRequest(const char* host,const int httpPort){
  WiFiClient client;

  //建立需要发送的数据信息
  String url = "/update?float=" + String(clientFloatValue) + 
               "&int=" + String(clientIntValue) +
               "&button=" + String(BOOTPinState);


  // 将需要发送的数据信息(url)放入字符串(httpRequest)中，用于HTTP请求,(GET既能获取数据,也能发送数据)
  //格式很重要 String("GET ") 这个中有一个空格,应该是不能省的,省略会导致HTTP请求发送不出去,很关键的
  String httpRequest =  String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";


  Serial.print(httpRequest);
  Serial.print("\n");

  //输出连接的网址
  Serial.print("connecting to :");
  Serial.print(host);
  Serial.print("\n");


  //连接网络服务器
  if(client.connect(host,httpPort)){
    //成功后输出success
    Serial.print("success\n");

    //向服务器发送HTTP请求
    client.print(httpRequest);    

    //串口输出HTTP请求信息
    Serial.print("sending request:");   
    Serial.print(httpRequest);
    Serial.print("\n");
  }
  else{
    Serial.print("connect failed!!!\n");
  }

  client.stop();    //断开与服务器的连接

    
}

void setup() {
  //连接串口
  Serial.begin(9600);
  Serial.print("serial is OK\n");

  //设置按键引脚
  pinMode(BOOTPin,INPUT);

  //wifi 连接设置
  wifi_multi_init();
  wifi_multi_con();
  Serial.print("wifi connected!!!\n");

  //输出连接信息(连接的WIFI名称及开发板的IP地址)
  Serial.print("\nconnect wifi:");
  Serial.print(WiFi.SSID());
  Serial.print("\n");
  Serial.print("\nIP address:");
  Serial.print(WiFi.localIP());
  Serial.print("\n");

}

void loop() {

  //读取BOOT引脚数据
  BOOTPinState = digitalRead(BOOTPin);
  Serial.print("BOOTPin value is:");
  Serial.print(BOOTPinState);
  Serial.print("\n");

  //这是相关的实验数据,不重要,只是测试用的
  clientFloatValue += 1.5;
  clientIntValue += 2;

  //同指定网址建立连接,并发送HTTP信息
  wifiClientRequest("192.168.0.123",80);

  delay(1000);

}