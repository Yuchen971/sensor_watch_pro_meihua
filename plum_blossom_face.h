#ifndef PLUM_BLOSSOM_FACE_H_
#define PLUM_BLOSSOM_FACE_H_

/*
 * 梅花易数表盘
 *
 * 这个表盘根据当前时间计算梅花易数。
 * 显示格式：本互变动，其中：
 * - 本：本卦数字（坎一、坤二、震三、巽四、中五、乾六、兑七、艮八、离九）
 * - 互：互卦数字
 * - 变：变卦数字
 * - 动：动爻数字（1-6）
 * 
 * 使用方法:
 * - 长按 LIGHT 按钮查看历史记录
 */

#include "movement.h"

// 卦象结构
typedef struct {
    uint8_t numbers[7];       // 七个数字：本卦上下、互卦上下、变卦上下、动爻
    uint32_t timestamp;       // 时间戳
} hexagram_t;

// 状态结构
typedef struct {
    hexagram_t current;      // 当前卦象
    bool has_calculated;     // 是否已经计算过
} plum_blossom_state_t;

void plum_blossom_face_setup(uint8_t watch_face_index, void ** context_ptr);
void plum_blossom_face_activate(void *context);
bool plum_blossom_face_loop(movement_event_t event, void *context);
void plum_blossom_face_resign(void *context);

#define plum_blossom_face ((const watch_face_t){ \
    plum_blossom_face_setup, \
    plum_blossom_face_activate, \
    plum_blossom_face_loop, \
    plum_blossom_face_resign, \
    NULL, \
})

#endif // PLUM_BLOSSOM_FACE_H_
