
#include "nvs_flash.h"
#include "lvgl_demo.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "led.h"
#include "lcd.h"
#include "lcd_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>

#define TAG "MAIN"

// 颜色测试函数
void color_test(void) 
{
    uint16_t colors[] = {
        RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, 
        WHITE, BLACK, GRAY, BROWN, LIGHTBLUE, LIGHTGREEN
    };
    
    const char* color_names[] = {
        "RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA",
        "WHITE", "BLACK", "GRAY", "BROWN", "LIGHTBLUE", "LIGHTGREEN"
    };
    
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    
    for (int i = 0; i < num_colors; i++) {
        ESP_LOGI(TAG, "Testing color: %s", color_names[i]);
        
        // 填充整个屏幕为当前颜色
        LCD_Fill(0, 0, LCD_W-1, LCD_H-1, colors[i]);
        
        // 在屏幕中央显示颜色名称
        if (colors[i] == WHITE || colors[i] == YELLOW || colors[i] == LIGHTBLUE || colors[i] == LIGHTGREEN) {
            // 亮色背景用黑字
            LCD_ShowString(LCD_W/2 - 30, LCD_H/2 - 8, color_names[i], BLACK, colors[i], 16, 1);
        } else {
            // 暗色背景用白字
            LCD_ShowString(LCD_W/2 - 30, LCD_H/2 - 8, color_names[i], WHITE, colors[i], 16, 1);
        }
        
        // 等待2秒
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

// 几何图形测试函数
void geometry_test(void)
{
    ESP_LOGI(TAG, "Testing geometry shapes");
    
    // 清屏为白色
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, WHITE);
    
    // 绘制不同颜色的矩形
    LCD_DrawFillRectangle(10, 10, 50, 50, RED);
    LCD_DrawFillRectangle(60, 10, 100, 50, GREEN);
    LCD_DrawFillRectangle(10, 60, 50, 100, BLUE);
    LCD_DrawFillRectangle(60, 60, 100, 100, YELLOW);
    
    // 绘制圆形
    LCD_DrawCircle(LCD_W/2, LCD_H/2, 30, MAGENTA, 1);
    LCD_DrawCircle(LCD_W/2, LCD_H/2, 20, CYAN, 1);
    LCD_DrawCircle(LCD_W/2, LCD_H/2, 10, RED, 1);
    
    // 绘制直线
    LCD_DrawLine(0, 0, LCD_W-1, LCD_H-1, BLACK);
    LCD_DrawLine(0, LCD_H-1, LCD_W-1, 0, BLACK);
    
    // 显示测试文字
    LCD_ShowString(5, LCD_H-30, "Geometry Test", BLACK, WHITE, 16, 1);
    
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}

// 渐变测试函数
void gradient_test(void)
{
    ESP_LOGI(TAG, "Testing color gradients");
    
    // 红色渐变
    for (int y = 0; y < LCD_H/3; y++) {
        uint16_t intensity = (y * 31) / (LCD_H/3);
        uint16_t color = intensity << 11; // 红色渐变
        for (int x = 0; x < LCD_W; x++) {
            LCD_DrawPoint(x, y, color);
        }
    }
    
    // 绿色渐变
    for (int y = LCD_H/3; y < 2*LCD_H/3; y++) {
        uint16_t intensity = ((y - LCD_H/3) * 63) / (LCD_H/3);
        uint16_t color = intensity << 5; // 绿色渐变
        for (int x = 0; x < LCD_W; x++) {
            LCD_DrawPoint(x, y, color);
        }
    }
    
    // 蓝色渐变
    for (int y = 2*LCD_H/3; y < LCD_H; y++) {
        uint16_t intensity = ((y - 2*LCD_H/3) * 31) / (LCD_H/3);
        uint16_t color = intensity; // 蓝色渐变
        for (int x = 0; x < LCD_W; x++) {
            LCD_DrawPoint(x, y, color);
        }
    }
    
    LCD_ShowString(5, 5, "RGB Gradients", WHITE, BLACK, 16, 1);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}

// RGB色彩条测试函数
void rgb_bars_test(void)
{
    ESP_LOGI(TAG, "Testing RGB color bars");
    
    int bar_width = LCD_W / 8;
    
    // 8个颜色条：黑、蓝、绿、青、红、洋红、黄、白
    uint16_t colors[] = {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE};
    const char* names[] = {"BLK", "BLU", "GRN", "CYN", "RED", "MAG", "YEL", "WHT"};
    
    for (int i = 0; i < 8; i++) {
        int x_start = i * bar_width;
        int x_end = (i == 7) ? LCD_W : (i + 1) * bar_width;
        
        LCD_DrawFillRectangle(x_start, 0, x_end-1, LCD_H-1, colors[i]);
        
        // 在每个色条上显示名称
        uint16_t text_color = (colors[i] == BLACK || colors[i] == BLUE || colors[i] == GREEN || colors[i] == RED) ? WHITE : BLACK;
        LCD_ShowString(x_start + 2, LCD_H/2 - 8, names[i], text_color, colors[i], 12, 1);
    }
    
    vTaskDelay(4000 / portTICK_PERIOD_MS);
}

// 色彩诊断测试 - 专门检测颜色错位
void color_diagnostic_test(void)
{
    ESP_LOGI(TAG, "Running color diagnostic test");
    
    // 测试纯色：应该能够明显看出颜色是否正确
    uint16_t pure_colors[] = {
        0xF800,  // 纯红色 (11111 000000 00000)
        0x07E0,  // 纯绿色 (00000 111111 00000)  
        0x001F,  // 纯蓝色 (00000 000000 11111)
        0xFFE0,  // 黄色 (红+绿)
        0xF81F,  // 洋红色 (红+蓝)
        0x07FF,  // 青色 (绿+蓝)
    };
    
    const char* pure_names[] = {"PURE RED", "PURE GREEN", "PURE BLUE", "YELLOW", "MAGENTA", "CYAN"};
    
    for (int i = 0; i < 6; i++) {
        // 分屏显示每种颜色
        LCD_Fill(0, 0, LCD_W-1, LCD_H-1, pure_colors[i]);
        
        // 显示颜色名称和RGB值
        uint16_t text_color = (i < 3) ? WHITE : BLACK;  // 纯色用白字，混合色用黑字
        
        LCD_ShowString(5, 20, pure_names[i], text_color, pure_colors[i], 16, 1);
        
        // 显示16位RGB值
        char rgb_str[20];
        sprintf(rgb_str, "0x%04X", pure_colors[i]);
        LCD_ShowString(5, 40, rgb_str, text_color, pure_colors[i], 12, 1);
        
        // 显示RGB分量
        char component_str[30];
        uint8_t r = (pure_colors[i] >> 11) & 0x1F;
        uint8_t g = (pure_colors[i] >> 5) & 0x3F;
        uint8_t b = pure_colors[i] & 0x1F;
        sprintf(component_str, "R:%d G:%d B:%d", r, g, b);
        LCD_ShowString(5, 60, component_str, text_color, pure_colors[i], 12, 1);
        
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

// 像素精确度测试
void pixel_accuracy_test(void)
{
    ESP_LOGI(TAG, "Testing pixel accuracy");
    
    // 清屏为黑色
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, BLACK);
    
    // 绘制边框测试 - 这将显示偏移是否正确
    LCD_DrawLine(0, 0, LCD_W-1, 0, RED);         // 上边框
    LCD_DrawLine(0, LCD_H-1, LCD_W-1, LCD_H-1, RED); // 下边框
    LCD_DrawLine(0, 0, 0, LCD_H-1, GREEN);       // 左边框
    LCD_DrawLine(LCD_W-1, 0, LCD_W-1, LCD_H-1, GREEN); // 右边框
    
    // 绘制四个角的单像素点
    LCD_DrawPoint(0, 0, WHITE);           // 左上角
    LCD_DrawPoint(LCD_W-1, 0, WHITE);     // 右上角
    LCD_DrawPoint(0, LCD_H-1, WHITE);     // 左下角
    LCD_DrawPoint(LCD_W-1, LCD_H-1, WHITE); // 右下角
    
    // 在中心绘制十字准线
    LCD_DrawLine(LCD_W/2, 0, LCD_W/2, LCD_H-1, YELLOW);
    LCD_DrawLine(0, LCD_H/2, LCD_W-1, LCD_H/2, YELLOW);
    
    // 绘制网格来检查偏移
    for (int x = 10; x < LCD_W; x += 20) {
        LCD_DrawLine(x, 0, x, LCD_H-1, BLUE);
    }
    
    for (int y = 10; y < LCD_H; y += 20) {
        LCD_DrawLine(0, y, LCD_W-1, y, BLUE);
    }
    
    // 显示屏幕尺寸信息
    char size_info[30];
    sprintf(size_info, "%dx%d", LCD_W, LCD_H);
    LCD_ShowString(5, 5, size_info, WHITE, BLACK, 16, 1);
    LCD_ShowString(5, 25, "Offset Test", WHITE, BLACK, 12, 1);
    
    // 显示当前方向
    char orientation[20];
    sprintf(orientation, "Orient: %d", USE_HORIZONTIAL);
    LCD_ShowString(5, 40, orientation, WHITE, BLACK, 12, 1);
    
    vTaskDelay(5000 / portTICK_PERIOD_MS);  // 延长显示时间以便观察
}

// 偏移量校准测试
void offset_calibration_test(void)
{
    ESP_LOGI(TAG, "Running offset calibration test");
    
    // 清屏为白色
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, WHITE);
    
    // 在屏幕的精确边界绘制测试图案
    // 这应该刚好贴边显示，如果有偏移就会看到位移
    
    // 左上角 5x5 红色方块
    LCD_DrawFillRectangle(0, 0, 4, 4, RED);
    
    // 右上角 5x5 绿色方块  
    LCD_DrawFillRectangle(LCD_W-5, 0, LCD_W-1, 4, GREEN);
    
    // 左下角 5x5 蓝色方块
    LCD_DrawFillRectangle(0, LCD_H-5, 4, LCD_H-1, BLUE);
    
    // 右下角 5x5 黄色方块
    LCD_DrawFillRectangle(LCD_W-5, LCD_H-5, LCD_W-1, LCD_H-1, YELLOW);
    
    // 中心十字
    LCD_DrawLine(LCD_W/2-10, LCD_H/2, LCD_W/2+10, LCD_H/2, BLACK);
    LCD_DrawLine(LCD_W/2, LCD_H/2-10, LCD_W/2, LCD_H/2+10, BLACK);
    
    // 显示偏移信息
    LCD_ShowString(10, LCD_H/2-40, "Calibration Test", BLACK, WHITE, 16, 1);
    LCD_ShowString(10, LCD_H/2-20, "Check corners", BLACK, WHITE, 12, 1);
    
    // 显示当前使用的偏移值（更新为0x0C）
    LCD_ShowString(10, LCD_H/2, "X_Offset: +LCD_X_OFFSET", BLACK, WHITE, 12, 1);
    LCD_ShowString(10, LCD_H/2+15, "Y_Offset: +LCD_Y_OFFSET", BLACK, WHITE, 12, 1);

    vTaskDelay(6000 / portTICK_PERIOD_MS);
}

// 边界像素测试 - 专门测试最右边和最下边的像素
void boundary_pixel_test(void)
{
    ESP_LOGI(TAG, "Testing boundary pixels - right edge and bottom edge");
    
    // 先填充整个屏幕为白色
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, WHITE);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    // 测试最右边2列
    LCD_Fill(LCD_W-2, 0, LCD_W-1, LCD_H-1, RED);
    LCD_ShowString(5, 20, "Right 2 cols RED", BLACK, WHITE, 16, 1);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    
    // 恢复白色背景，测试最下边2行
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, WHITE);
    LCD_Fill(0, LCD_H-2, LCD_W-1, LCD_H-1, BLUE);
    LCD_ShowString(5, 20, "Bottom 2 rows BLUE", BLACK, WHITE, 16, 1);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    
    // 测试右下角4个像素
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, WHITE);
    LCD_Fill(LCD_W-2, LCD_H-2, LCD_W-1, LCD_H-1, MAGENTA);
    LCD_ShowString(5, 20, "Bottom-right corner", BLACK, WHITE, 16, 1);
    LCD_ShowString(5, 40, "4 pixels MAGENTA", BLACK, WHITE, 16, 1);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    
    // 显示坐标信息
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, BLACK);
    LCD_ShowString(5, 20, "Screen Info:", WHITE, BLACK, 16, 1);
    char info[50];
    sprintf(info, "Size: %dx%d", LCD_W, LCD_H);
    LCD_ShowString(5, 40, info, WHITE, BLACK, 12, 1);
    sprintf(info, "Max coords: (%d,%d)", LCD_W-1, LCD_H-1);
    LCD_ShowString(5, 60, info, WHITE, BLACK, 12, 1);
    sprintf(info, "X_Offset: 0x%02X", LCD_X_OFFSET);
    LCD_ShowString(5, 80, info, WHITE, BLACK, 12, 1);
    sprintf(info, "Y_Offset: 0x%02X", LCD_Y_OFFSET);
    LCD_ShowString(5, 100, info, WHITE, BLACK, 12, 1);
    
    vTaskDelay(4000 / portTICK_PERIOD_MS);
}

// 边缘杂色诊断测试
void edge_artifact_test(void)
{
    ESP_LOGI(TAG, "Running edge artifact diagnosis test");
    
    // 测试1：纯色填充边缘测试
    uint16_t test_colors[] = {BLACK, WHITE, RED, GREEN, BLUE, YELLOW};
    const char* color_names[] = {"BLACK", "WHITE", "RED", "GREEN", "BLUE", "YELLOW"};
    
    for (int i = 0; i < 6; i++) {
        ESP_LOGI(TAG, "Testing edge artifacts with %s", color_names[i]);
        
        // 完整屏幕填充
        LCD_Fill(0, 0, LCD_W-1, LCD_H-1, test_colors[i]);
        
        // 在中心显示颜色信息
        uint16_t text_color = (test_colors[i] == BLACK || test_colors[i] == BLUE || test_colors[i] == RED) ? WHITE : BLACK;
        LCD_ShowString(LCD_W/2-40, LCD_H/2-10, "EDGE TEST", text_color, test_colors[i], 16, 1);
        LCD_ShowString(LCD_W/2-30, LCD_H/2+10, color_names[i], text_color, test_colors[i], 12, 1);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    
    // 测试2：边框测试
    LCD_Fill(0, 0, LCD_W-1, LCD_H-1, WHITE);
    
    // 绘制1像素边框
    for (int x = 0; x < LCD_W; x++) {
        LCD_DrawPoint(x, 0, RED);          // 上边框
        LCD_DrawPoint(x, LCD_H-1, RED);    // 下边框
    }
    for (int y = 0; y < LCD_H; y++) {
        LCD_DrawPoint(0, y, GREEN);        // 左边框
        LCD_DrawPoint(LCD_W-1, y, GREEN);  // 右边框
    }
    
    LCD_ShowString(10, 20, "Edge Border Test", BLACK, WHITE, 16, 1);
    LCD_ShowString(10, 40, "Red: Top/Bottom", BLACK, WHITE, 12, 1);
    LCD_ShowString(10, 55, "Green: Left/Right", BLACK, WHITE, 12, 1);
    
    vTaskDelay(4000 / portTICK_PERIOD_MS);
}

// 综合测试任务
void display_test_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Starting display test loop");
    
    // 等待LCD初始化完成
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    while (1) {
        ESP_LOGI(TAG, "=== Starting new test cycle ===");
        
        // 1. 偏移量校准测试 - 最重要的测试
        offset_calibration_test();
        
        // 2. 像素精确度测试
        pixel_accuracy_test();
        
        // 3. 色彩诊断测试
        color_diagnostic_test();
        
        // 4. RGB色彩条测试
        rgb_bars_test();
        
        // 5. 颜色测试
        color_test();
        
        // 6. 几何图形测试
        geometry_test();
        
        // 7. 渐变测试
        gradient_test();
        
        ESP_LOGI(TAG, "=== Test cycle completed ===");
        
        // 等待3秒后开始下一轮测试
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief       程序入口
 * @param       无
 * @retval      无
 */
void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 2.7 inch NV3007 TFT Display Test Starting");
    ESP_LOGI(TAG, "Display Resolution: 142x428 pixels");
    ESP_LOGI(TAG, "Buffer Size: %d bytes", LCD_BUF_SIZE);
    
    // 初始化SPI和LCD显示
    LCD_Init();
    
    ESP_LOGI(TAG, "Display initialized successfully");
    
    int test_cycle = 0;
    
    while (1) {
        ESP_LOGI(TAG, "\n=== Starting test cycle %d ===", ++test_cycle);
        
        // 0. 边界像素测试 - 最重要的测试，放在第一位
        boundary_pixel_test();
        
        // 1. 基础颜色测试 (使用已定义的函数)
        color_test();
        
        // 2. 几何形状测试 (使用已定义的函数)
        geometry_test();
        
        // 3. 渐变效果测试
        gradient_test();
        
        // 4. 像素精度测试 (使用已定义的函数)
        pixel_accuracy_test();
        
        // 5. 偏移量校准测试
        offset_calibration_test();
        
        // 6. 边缘杂色诊断测试 (新增)
        edge_artifact_test();
        
        // 等待后开始下一轮测试
        ESP_LOGI(TAG, "Test cycle %d completed, waiting before next cycle", test_cycle);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
  