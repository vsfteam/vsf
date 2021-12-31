/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_FOREACH_H__
#define __VSF_FOREACH_H__

#include "vsf_macro_calc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __VSF_MEMPTY()
#define __VSF_MDEFER(__X)                       __X __VSF_MEMPTY()
#define __VSF_MOBSTRUCT(...)                    __VA_ARGS__ __VSF_MDEFER(__VSF_MEMPTY)()
#define __VSF_MEXPAND_ONCE(...)                 __VA_ARGS__
#define __VSF_MEAT(...)

#define __VSF_MEXPAND(...)                      __VSF_MEXPAND1(__VSF_MEXPAND1(__VSF_MEXPAND1(__VSF_MEXPAND1(__VA_ARGS__))))
#define __VSF_MEXPAND1(...)                     __VSF_MEXPAND2(__VSF_MEXPAND2(__VSF_MEXPAND2(__VSF_MEXPAND2(__VA_ARGS__))))
#define __VSF_MEXPAND2(...)                     __VSF_MEXPAND3(__VSF_MEXPAND3(__VSF_MEXPAND3(__VSF_MEXPAND3(__VA_ARGS__))))
#define __VSF_MEXPAND3(...)                     __VSF_MEXPAND4(__VSF_MEXPAND4(__VSF_MEXPAND4(__VSF_MEXPAND4(__VA_ARGS__))))
#define __VSF_MEXPAND4(...)                     __VSF_MEXPAND5(__VSF_MEXPAND5(__VSF_MEXPAND5(__VSF_MEXPAND5(__VA_ARGS__))))
#define __VSF_MEXPAND5(...)                     __VA_ARGS__


#define ____VSF_IS_EMPTY(__A, __B, __SIZE, ...) __SIZE
#define __VSF_IS_EMPTY(...)                     ____VSF_IS_EMPTY(, ##__VA_ARGS__, 0, 1)
#define VSF_IS_EMPTY(__X, ...)                  __VSF_IS_EMPTY(__X)

#define __VSF_MFOREACH(__MACRO, __MACRO_LAST, __X, ...)                          \
            VSF_MCAT2(__VSF_MFOREACH_, VSF_IS_EMPTY(__VA_ARGS__))(__MACRO, __MACRO_LAST, __X, __VA_ARGS__)
#define __VSF_MFOREACH_0(__MACRO, __MACRO_LAST, __X, ...)                        \
            __MACRO(__X) __VSF_MOBSTRUCT(__VSF_MFOREACH_I)()(__MACRO, __MACRO_LAST, __VA_ARGS__)
#define __VSF_MFOREACH_1(__MACRO, __MACRO_LAST, __X, ...)                        \
            __MACRO_LAST(__X)
#define __VSF_MFOREACH_I()                      __VSF_MFOREACH
#define VSF_MFOREACH_EX(__MACRO, __MACRO_LAST, ...)                             \
            __VSF_MEXPAND(__VSF_MFOREACH(__MACRO, __MACRO_LAST, __VA_ARGS__))
#define VSF_MFOREACH(__MACRO, ...)              VSF_MFOREACH_EX(__MACRO, __MACRO, __VA_ARGS__)




/*   elegant but toooooo slow
#define ____VSF_MCHECK(__X, __SIZE, ...)        __SIZE
#define __VSF_MCHECK(...)                       ____VSF_MCHECK(__VA_ARGS__, 0,)

#define __VSF_MNOT(__X)                         __VSF_MCHECK(VSF_MCAT2_PRIMITIVE(__VSF_MNOT_, __X))
#define __VSF_MNOT_0                            __VSF_MDUMMY, 1,

#define __VSF_MCOMPL(__X)                       VSF_MCAT2_PRIMITIVE(__VSF_MCOMPL_, __X)
#define __VSF_MCOMPL_0                          1
#define __VSF_MCOMPL_1                          0

#define __VSF_MBOOL(__X)                        __VSF_MCOMPL(__VSF_MNOT(__X))

#define ____VSF_MIF(__COND)                     VSF_MCAT2_PRIMITIVE(__VSF_IF_, __COND)
#define __VSF_IF_0(__X, ...)                    __VA_ARGS__
#define __VSF_IF_1(__X, ...)                    __X

#define __VSF_MIF(__COND)                       ____VSF_MIF(__VSF_MBOOL(__COND))
#define __VSF_MWHEN(__COND)                     __VSF_MIF(__COND)(__VSF_MEXPAND_ONCE, __VSF_MEAT)

#define __VSF_MREPEAT(__COUNT, __MACRO, ...)                                    \
    __VSF_MWHEN(__COUNT) (                                                      \
        __VSF_MOBSTRUCT(__VSF_MREPEAT_I)() (                                    \
            VSF_MDEC(__COUNT), __MACRO, __VA_ARGS__                             \
        )                                                                       \
        __VSF_MOBSTRUCT(__MACRO)(VSF_MDEC(__COUNT), __VA_ARGS__)                \
    )
#define __VSF_MREPEAT_I()                       __VSF_MREPEAT
#define VSF_MREPEAT(__COUNT, __MACRO, ...)                                      \
            __VSF_MEXPAND(__VSF_MREPEAT(__COUNT, __MACRO, __VA_ARGS__))
#define VSF_MREPEAT_LIMIT                       VSF_MDEC_LIMIT
*/

#ifdef __cplusplus
}
#endif

#endif
