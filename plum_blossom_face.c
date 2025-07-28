#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "plum_blossom_face.h"
#include "watch.h"
#include "watch_utility.h"

// 先天八卦爻象
static const uint8_t TRIGRAM_LINES[9][3] = {
    {0, 0, 0},  // 占位，不使用
    {1, 1, 1},  // 乾 ☰
    {0, 1, 1},  // 兑 ☱
    {1, 0, 1},  // 离 ☲
    {0, 0, 1},  // 震 ☳
    {1, 1, 0},  // 巽 ☴
    {0, 1, 0},  // 坎 ☵
    {1, 0, 0},  // 艮 ☶
    {0, 0, 0}   // 坤 ☷
};

// 先天八卦名称
static const char* TRIGRAM_NAMES_C[] = {
    "错误", "乾", "兑", "离", "震", "巽", "坎", "艮", "坤"
}; //索引0为错误/占位，1-8对应卦名

// 获取六十四卦名称的函数
static const char* get_hexagram_name_c(uint8_t upper_num, uint8_t lower_num) {
    if (upper_num < 1 || upper_num > 8 || lower_num < 1 || lower_num > 8) {
        return "未知卦";
    }
    static const char* H_NAMES[9][9] = {
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, // 索引0占位
        // 上卦为乾 (1)           下卦:乾1     兑2     离3      震4      巽5      坎6      艮7      坤8
        {NULL, "乾为天", "天泽履", "天火同人", "天雷无妄", "天风姤", "天水讼", "天山遁", "天地否"},
        // 上卦为兑 (2)
        {NULL, "泽天夬", "兑为泽", "泽火革", "泽雷随", "泽风大过", "泽水困", "泽山咸", "泽地萃"},
        // 上卦为离 (3)
        {NULL, "火天大有", "火泽睽", "离为火", "火雷噬嗑", "火风鼎", "火水未济", "火山旅", "火地晋"},
        // 上卦为震 (4)
        {NULL, "雷天大壮", "雷泽归妹", "雷火丰", "震为雷", "雷风恒", "雷水解", "雷山小过", "雷地豫"},
        // 上卦为巽 (5)
        {NULL, "风天小畜", "风泽中孚", "风火家人", "风雷益", "巽为风", "风水涣", "风山渐", "风地观"},
        // 上卦为坎 (6)
        {NULL, "水天需", "水泽节", "水火既济", "水雷屯", "水风井", "坎为水", "水山蹇", "水地比"},
        // 上卦为艮 (7)
        {NULL, "山天大畜", "山泽损", "山火贲", "山雷颐", "山风蛊", "山水蒙", "艮为山", "山地剥"},
        // 上卦为坤 (8)
        {NULL, "地天泰", "地泽临", "地火明夷", "地雷复", "地风升", "地水师", "地山谦", "坤为地"}
    };
    if (H_NAMES[upper_num][lower_num] == NULL) {
        return "定义错误";
    }
    return H_NAMES[upper_num][lower_num];
}

// 农历数据表 1900-2100 年
static const uint32_t LUNAR_INFO[] = {
    0x04bd8, 0x04ae0, 0x0a570, 0x054d5, 0x0d260, 0x0d950, 0x16554, 0x056a0, 0x09ad0, 0x055d2, // 1900-1909
    0x04ae0, 0x0a5b6, 0x0a4d0, 0x0d250, 0x1d255, 0x0b540, 0x0d6a0, 0x0ada2, 0x095b0, 0x14977, // 1910-1919
    0x04970, 0x0a4b0, 0x0b4b5, 0x06a50, 0x06d40, 0x1ab54, 0x02b60, 0x09570, 0x052f2, 0x04970, // 1920-1929
    0x06566, 0x0d4a0, 0x0ea50, 0x06e95, 0x05ad0, 0x02b60, 0x186e3, 0x092e0, 0x1c8d7, 0x0c950, // 1930-1939
    0x0d4a0, 0x1d8a6, 0x0b550, 0x056a0, 0x1a5b4, 0x025d0, 0x092d0, 0x0d2b2, 0x0a950, 0x0b557, // 1940-1949
    0x06ca0, 0x0b550, 0x15355, 0x04da0, 0x0a5d0, 0x14573, 0x052d0, 0x0a9a8, 0x0e950, 0x06aa0, // 1950-1959
    0x0aea6, 0x0ab50, 0x04b60, 0x0aae4, 0x0a570, 0x05260, 0x0f263, 0x0d950, 0x05b57, 0x056a0, // 1960-1969
    0x096d0, 0x04dd5, 0x04ad0, 0x0a4d0, 0x0d4d4, 0x0d250, 0x0d558, 0x0b540, 0x0b5a0, 0x195a6, // 1970-1979
    0x095b0, 0x049b0, 0x0a974, 0x0a4b0, 0x0b27a, 0x06a50, 0x06d40, 0x0af46, 0x0ab60, 0x09570, // 1980-1989
    0x04af5, 0x04970, 0x064b0, 0x074a3, 0x0ea50, 0x06b58, 0x055c0, 0x0ab60, 0x096d5, 0x092e0, // 1990-1999
    0x0c960, 0x0d954, 0x0d4a0, 0x0da50, 0x07552, 0x056a0, 0x0abb7, 0x025d0, 0x092d0, 0x0cab5, // 2000-2009
    0x0a950, 0x0b4a0, 0x0baa4, 0x0ad50, 0x055d9, 0x04ba0, 0x0a5b0, 0x15176, 0x052b0, 0x0a930, // 2010-2019
    0x07954, 0x06aa0, 0x0ad50, 0x05b52, 0x04b60, 0x0a6e6, 0x0a4e0, 0x0d260, 0x0ea65, 0x0d530, // 2020-2029
    0x05aa0, 0x076a3, 0x096d0, 0x04bd7, 0x04ad0, 0x0a4d0, 0x1d0b6, 0x0d250, 0x0d520, 0x0dd45, // 2030-2039
    0x0b5a0, 0x056d0, 0x055b2, 0x049b0, 0x0a577, 0x0a4b0, 0x0aa50, 0x1b255, 0x06d20, 0x0ada0, // 2040-2049
    0x14b63, 0x09370, 0x049f8, 0x04970, 0x064b0, 0x168a6, 0x0ea50, 0x06aa0, 0x1a6c4, 0x0aae0, // 2050-2059
    0x092e0, 0x0d2e3, 0x0c960, 0x0d557, 0x0d4a0, 0x0da50, 0x05d55, 0x056a0, 0x0a6d0, 0x055d4, // 2060-2069
    0x052d0, 0x0a9b8, 0x0a950, 0x0b4a0, 0x0b6a6, 0x0ad50, 0x055a0, 0x0aba4, 0x0a5b0, 0x052b0, // 2070-2079
    0x0b273, 0x06930, 0x07337, 0x06aa0, 0x0ad50, 0x14b55, 0x04b60, 0x0a570, 0x054e4, 0x0d260, // 2080-2089
    0x0e968, 0x0d520, 0x0daa0, 0x16aa6, 0x056d0, 0x04ae0, 0x0a9d4, 0x0a4d0, 0x0d150, 0x0f252, // 2090-2099
    0x0d520                                                                                     // 2100
};

// 计算农历年的总天数
static uint16_t _lunar_year_days(uint16_t year) {
    uint32_t i, sum = 348;
    if (year < 1900 || year > 2100) {
        // printf("调试_LYD: year %u out of range, returning 365\n", year); // Optional: uncomment for more verbosity
        return 365;
    }
    uint32_t info = LUNAR_INFO[year - 1900];
    for (i = 0x8000; i > 0x8; i >>= 1) { sum += (info & i) ? 1 : 0; }
    uint8_t leap_month = info & 0xf;
    if (leap_month) { sum += (info & 0x10000) ? 30 : 29; }
    // printf("调试_LYD: year %u, info 0x%lx, sum_days %u\n", year, info, sum); // Optional: uncomment for more verbosity
    return sum;
}

// 计算闰月，没有则返回0
static uint8_t _leap_month(uint16_t year) {
    if (year < 1900 || year > 2100) return 0; // 超出数据范围
    return LUNAR_INFO[year - 1900] & 0xf;
}

// 计算闰月天数，没有闰月则返回0
static uint8_t _leap_days(uint16_t year) {
    if (!_leap_month(year)) return 0;
    if (year < 1900 || year > 2100) return 0; // 超出数据范围
    return (LUNAR_INFO[year - 1900] & 0x10000) ? 30 : 29;
}

// 计算农历月的天数
static uint8_t _lunar_month_days(uint16_t year, uint8_t month) {
    if (month > 12 || month < 1) return 0;
    if (year < 1900 || year > 2100) return 30; // 超出数据范围，返回默认值
    
    return (LUNAR_INFO[year - 1900] & (0x10000 >> month)) ? 30 : 29;
}

// 公历转农历
static void _solar_to_lunar(uint16_t year, uint8_t month, uint8_t day, uint16_t *l_year, uint8_t *l_month, uint8_t *l_day) {
    // 初始值设为公历值（以防转换失败）
    *l_year = year;
    *l_month = month;
    *l_day = day;
    
    // 如果年份超出范围，直接返回
    if (year < 1900 || year > 2100) {
        printf("调试_STL: 年份 %u 超出支持范围(1900-2100)，使用公历作为农历近似值。\n", year);
        printf("调试_STL: 农历(近似): 年 %u, 月 %u, 日 %u\n", *l_year, *l_month, *l_day);
        return;
    }
    
    // 计算从1900年1月31日开始的天数偏移
    // 这是农历1900年正月初一，我们的数据起点
    
    // 先计算当前日期是从1900年1月1日开始的第几天
    int offset_days = 0;
    
    // 加上年数的贡献
    for (int y = 1900; y < year; y++) {
        offset_days += ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ? 366 : 365;
    }
    
    // 加上当年已过去的月份天数
    for (int m = 1; m < month; m++) {
        if (m == 2) { // 2月
            offset_days += ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 29 : 28;
        } else if (m == 4 || m == 6 || m == 9 || m == 11) { // 小月
            offset_days += 30;
        } else { // 大月
            offset_days += 31;
        }
    }
    
    // 加上当月天数
    offset_days += day;
    
    // 减去1900年1月1日到1900年1月31日的天数
    // 1月31日是1月的第31天，1月1日是第1天，所以差距是30天
    // 农历1900年正月初一是从1900年1月31日开始的
    // 但可能需要调整这个偏移量
    offset_days -= 31; // 修正：改为31，因为我们需要正好从1月31日开始算
    
    printf("调试_STL: 从1900年1月31日到 %u年%u月%u日 的天数: %d\n", year, month, day, offset_days);
    
    if (offset_days < 0) {
        printf("调试_STL: 日期在农历1900年正月初一之前，无法转换\n");
        return;
    }
    
    // 计算农历年
    uint16_t lunar_year = 1900;
    int days_in_lunar_year = 0;
    
    while (offset_days >= 0) {
        days_in_lunar_year = _lunar_year_days(lunar_year);
        if (offset_days < days_in_lunar_year) {
            break;
        }
        offset_days -= days_in_lunar_year;
        lunar_year++;
    }
    
    printf("调试_STL: 计算得到农历年份: %u, 剩余天数: %d\n", lunar_year, offset_days);
    
    // 计算农历月份和日期
    uint8_t lunar_month = 1;
    uint8_t leap_month = _leap_month(lunar_year);
    bool is_leap = false;
    
    // 计算月
    int days_in_month = 0;
    
    // 对于农历年的每个月
    while (offset_days >= 0) {
        // 处理闰月
        if (leap_month > 0 && lunar_month == leap_month + 1 && !is_leap) {
            // 进入闰月
            lunar_month--; // 回到上个月的闰月
            is_leap = true;
            days_in_month = _leap_days(lunar_year);
        } else {
            days_in_month = _lunar_month_days(lunar_year, lunar_month);
            is_leap = false;
        }
        
        printf("调试_STL: 检查农历月: %u%s, 天数: %d, 剩余偏移: %d\n", 
               lunar_month, is_leap ? "(闰)" : "", days_in_month, offset_days);
        
        if (offset_days < days_in_month) {
            break;
        }
        
        offset_days -= days_in_month;
        
        if (is_leap) {
            is_leap = false; // 闰月结束
        } else {
            lunar_month++; // 进入下一个月
            if (lunar_month > 12) {
                printf("警告_STL: 农历月超过12，可能有计算错误\n");
                break;
            }
        }
    }
    
    // 计算日
    uint8_t lunar_day = offset_days + 1; // 日期从1开始
    
    *l_year = lunar_year;
    *l_month = lunar_month;
    *l_day = lunar_day;
    
    printf("调试_STL: 公历 %u年%u月%u日 => 农历 %u年%u月%u日%s\n", 
           year, month, day, *l_year, *l_month, *l_day, is_leap ? "(闰)" : "");
}

// 计算年支数字
static uint8_t _get_year_branch_num(uint16_t lunar_year) {
    return (lunar_year - 4) % 12 + 1;
}

// 计算时辰支数字
static uint8_t _get_hour_branch_num(uint8_t solar_hour) {
    if (solar_hour == 23) return 1; // 子时
    return ((solar_hour + 1) / 2 % 12) + 1;
}

// 根据爻象查找卦数
static uint8_t _find_trigram_by_lines(const uint8_t lines[3]) {
    for (uint8_t i = 1; i <= 8; i++) {
        if (lines[0] == TRIGRAM_LINES[i][0] && 
            lines[1] == TRIGRAM_LINES[i][1] && 
            lines[2] == TRIGRAM_LINES[i][2]) {
            return i;
        }
    }
    return 0; // 应该不会发生
}

// 计算并返回7个数字
static void _calculate_plum_blossom_numbers(watch_date_time_t *datetime, uint8_t *numbers) {
    printf("梅花易数计算开始...\n");

    // RTC 年份是相对于 WATCH_RTC_REFERENCE_YEAR (2020) 的偏移量
    uint16_t solar_year = datetime->unit.year + WATCH_RTC_REFERENCE_YEAR;
    printf("调试: RTC年份偏移 %u + 基准年 %d = 公历年份 %u\n", 
           datetime->unit.year, WATCH_RTC_REFERENCE_YEAR, solar_year);

    // 确保年份在支持范围内
    if (solar_year < 1900 || solar_year > 2100) {
        printf("警告: 公历年份 %u 超出支持范围(1900-2100)，将使用2025年代替\n", solar_year);
        solar_year = 2025; // 使用一个合理的默认年份
    }

    printf("公历输入 (调整后): 年 %u, 月 %u, 日 %u, 时 %u, 分 %u, 秒 %u\n",
           solar_year, datetime->unit.month, datetime->unit.day,
           datetime->unit.hour, datetime->unit.minute, datetime->unit.second);

    uint16_t lunar_year;
    uint8_t lunar_month, lunar_day;
    
    uint8_t solar_month = datetime->unit.month;
    uint8_t solar_day = datetime->unit.day;
    uint8_t solar_hour = datetime->unit.hour;
    
    _solar_to_lunar(solar_year, solar_month, solar_day, &lunar_year, &lunar_month, &lunar_day);
    // _solar_to_lunar 内部已打印农历结果

    uint8_t year_branch_num = _get_year_branch_num(lunar_year);
    uint8_t hour_branch_num = _get_hour_branch_num(solar_hour);
    printf("计算参数: 年支数=%u, 农历月=%u, 农历日=%u, 时支数=%u\n",
           year_branch_num, lunar_month, lunar_day, hour_branch_num);
    
    uint8_t sum_for_upper = year_branch_num + lunar_month + lunar_day;
    uint8_t primary_upper = sum_for_upper % 8;
    if (primary_upper == 0) primary_upper = 8;
    printf("上卦计算: (%u + %u + %u) = %u => %u %% 8 = %u (%s)\n",
           year_branch_num, lunar_month, lunar_day, sum_for_upper, sum_for_upper, primary_upper, TRIGRAM_NAMES_C[primary_upper]);
    
    uint8_t sum_for_lower_changing = sum_for_upper + hour_branch_num;
    uint8_t primary_lower = sum_for_lower_changing % 8;
    if (primary_lower == 0) primary_lower = 8;
    
    uint8_t changing_line = sum_for_lower_changing % 6;
    if (changing_line == 0) changing_line = 6;
    printf("下卦计算: (%u + %u) = %u => %u %% 8 = %u (%s)\n",
           sum_for_upper, hour_branch_num, sum_for_lower_changing, sum_for_lower_changing, primary_lower, TRIGRAM_NAMES_C[primary_lower]);
    printf("动爻计算: %u %% 6 = %u\n", sum_for_lower_changing, changing_line);
    
    numbers[0] = primary_upper;
    numbers[1] = primary_lower;
    
    uint8_t full_hexagram[6];
    memcpy(full_hexagram, TRIGRAM_LINES[primary_upper], 3); // 上卦爻象
    memcpy(full_hexagram + 3, TRIGRAM_LINES[primary_lower], 3); // 下卦爻象
    printf("本卦爻象: %d%d%d %d%d%d\n", full_hexagram[0],full_hexagram[1],full_hexagram[2],full_hexagram[3],full_hexagram[4],full_hexagram[5]);

    uint8_t mutual_upper_lines[3] = {full_hexagram[1], full_hexagram[2], full_hexagram[3]};
    numbers[2] = _find_trigram_by_lines(mutual_upper_lines);
    
    uint8_t mutual_lower_lines[3] = {full_hexagram[2], full_hexagram[3], full_hexagram[4]};
    numbers[3] = _find_trigram_by_lines(mutual_lower_lines);
    
    uint8_t changed_hexagram_lines[6];
    memcpy(changed_hexagram_lines, full_hexagram, 6);
    if (changing_line > 0 && changing_line <=6) { // Ensure changing_line is valid
        uint8_t change_idx = 6 - changing_line; // 0 to 5
        changed_hexagram_lines[change_idx] = 1 - changed_hexagram_lines[change_idx];
    } else {
        printf("错误: 动爻 %d 超出范围 (1-6)，无法变卦！\n", changing_line);
    }
    
    uint8_t changed_upper_trigram_lines[3] = {changed_hexagram_lines[0], changed_hexagram_lines[1], changed_hexagram_lines[2]};
    numbers[4] = _find_trigram_by_lines(changed_upper_trigram_lines);
    
    uint8_t changed_lower_trigram_lines[3] = {changed_hexagram_lines[3], changed_hexagram_lines[4], changed_hexagram_lines[5]};
    numbers[5] = _find_trigram_by_lines(changed_lower_trigram_lines);
    
    numbers[6] = changing_line;

    printf("--- 计算结果 ---\n");
    printf("本卦: %s (%u) / %s (%u) - %s\n",
           TRIGRAM_NAMES_C[numbers[0]], numbers[0], TRIGRAM_NAMES_C[numbers[1]], numbers[1], get_hexagram_name_c(numbers[0], numbers[1]));
    printf("互卦: %s (%u) / %s (%u) - %s\n",
           TRIGRAM_NAMES_C[numbers[2]], numbers[2], TRIGRAM_NAMES_C[numbers[3]], numbers[3], get_hexagram_name_c(numbers[2], numbers[3]));
    printf("变卦: %s (%u) / %s (%u) - %s\n",
           TRIGRAM_NAMES_C[numbers[4]], numbers[4], TRIGRAM_NAMES_C[numbers[5]], numbers[5], get_hexagram_name_c(numbers[4], numbers[5]));
    printf("动爻: %u\n", numbers[6]);
    printf("原始七数输出 (本上,本下,互上,互下,变上,变下,动): %u,%u,%u,%u,%u,%u,%u\n",
           numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], numbers[5], numbers[6]);
    printf("梅花易数计算结束.\n\n");
}

void plum_blossom_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    (void) watch_face_index;
    
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(plum_blossom_state_t));
        memset(*context_ptr, 0, sizeof(plum_blossom_state_t));
    }
}

void plum_blossom_face_activate(void *context) {
    plum_blossom_state_t *state = (plum_blossom_state_t *)context;
    
    // 每次激活时都清空之前的记录
    state->has_calculated = false;
    memset(state->current.numbers, 0, sizeof(state->current.numbers));
    state->current.timestamp = 0;
}

// 显示梅花易数结果
static void _display_plum_blossom(const uint8_t *numbers, bool has_calculated) {
    char buf[14]; // Buffer is large enough for 10 chars + null
    
    if (!has_calculated) {
        // 显示提示信息：MH READY（需要8个字符对齐）
        sprintf(buf, "MH  READY");
    } else {
        // 将所有梅花易数信息格式化到一个字符串中
        // MH (2) + 动爻(1) + 本上(1) + 本下(1) + 互上(1) + 互下(1) + 变上(1) + 变下(1) = 9 chars
        sprintf(buf, "MH %1d%1d%1d%1d%1d%1d%1d", 
                numbers[6], // 动爻
                numbers[0], // 本卦上卦
                numbers[1], // 本卦下卦
                numbers[2], // 互卦上卦
                numbers[3], // 互卦下卦
                numbers[4], // 变卦上卦
                numbers[5]  // 变卦下卦
               );
    }
    
    // 从位置0开始显示整个字符串
    watch_display_string(buf, 0);
}

bool plum_blossom_face_loop(movement_event_t event, void *context) {
    plum_blossom_state_t *state = (plum_blossom_state_t *)context;
    
    switch (event.event_type) {
        case EVENT_ACTIVATE:
        case EVENT_TICK:
            // 显示当前卦象
            _display_plum_blossom(state->current.numbers, state->has_calculated);
            break;
            
        case EVENT_LIGHT_BUTTON_DOWN:
            movement_illuminate_led();
            break;
            
        case EVENT_LIGHT_BUTTON_UP:
            {
                // 按 LIGHT 按钮开始计算梅花易数
                watch_date_time_t datetime = movement_get_local_date_time();
                printf("调试: EVENT_LIGHT_BUTTON_UP: 开始计算梅花易数\n");
                _calculate_plum_blossom_numbers(&datetime, state->current.numbers);
                state->current.timestamp = watch_utility_date_time_to_unix_time(datetime, movement_get_current_timezone_offset());
                state->has_calculated = true;
            }
            break;
            
        case EVENT_ALARM_BUTTON_DOWN:
            watch_buzzer_play_note(BUZZER_NOTE_C8, 50);
            break;
            
        case EVENT_ALARM_BUTTON_UP:
            {
                // 重新计算
                watch_date_time_t datetime = movement_get_local_date_time();
                printf("调试: EVENT_ALARM_BUTTON_UP: 重新计算梅花易数\n");
                _calculate_plum_blossom_numbers(&datetime, state->current.numbers);
                state->current.timestamp = watch_utility_date_time_to_unix_time(datetime, movement_get_current_timezone_offset());
                state->has_calculated = true;
                
                watch_buzzer_play_note(BUZZER_NOTE_E8, 50);
            }
            break;
            
        case EVENT_MODE_BUTTON_UP:
            // 返回到上一个表盘
            movement_move_to_next_face();
            break;
            
        case EVENT_TIMEOUT:
            // 超时保持在当前表盘
            break;
            
        case EVENT_LOW_ENERGY_UPDATE:
            // 低能耗模式下，显示静态信息
            watch_display_string("MH      ", 0);
            break;
            
        default:
            return movement_default_loop_handler(event);
    }
    
    return true;
}

void plum_blossom_face_resign(void *context) {
    (void) context;
}
