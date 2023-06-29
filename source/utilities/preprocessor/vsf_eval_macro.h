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

#ifndef __VSF_EVAL_H__
#define __VSF_EVAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define __VSF_VA_NUM_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,__N,...)      __N

#define VSF_VA_NUM_ARGS(...)                    __VSF_VA_NUM_ARGS_IMPL(0,##__VA_ARGS__,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define __VSF_EVAL_MACRO_NAME(__FUNC, __ARGC)   __FUNC##__ARGC
#define VSF_EVAL_MACRO(__FUNC, ...)             __VSF_EVAL_MACRO_NAME(__FUNC, VSF_VA_NUM_ARGS(__VA_ARGS__))

#ifdef __cplusplus
}
#endif

#endif
