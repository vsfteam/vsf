#include "scgui.h"

// sc_lcd.c

/**fun: 快速初始化*/
static inline void SC_pfb_init(SC_tile *dest,uint32_t colour)
{
    int len=dest->w*dest->h;
    colour=colour<<16|colour;
    for(int i=0; i<len/2; i++)
    {
        ((uint32_t *)dest->buf)[i]=colour;
    }
    dest->buf[len-1]=colour;
}

/**fun: pfb分割*/
void SC_pfb_clip(SC_tile *clip, int xs,int ys,int xe,int ye,uint16_t colour)
{
    static uint16_t addr=0;
    uint16_t width= xe-xs+1;
    uint16_t height=ye-ys+1;
    if(xs<0) xs=0;
    if(ys<0) ys=0;
    int max_height =   SC_PFB_BUF_SIZE/(width*2); // 根据帧缓冲区大小计算最大高度
    clip->num = (height + max_height - 1) / max_height; // 向上取整
    if(clip->stup<clip->num)
    {
        addr=addr>0? 0: SC_PFB_BUF_SIZE/2;
        clip->buf= &gui->pfb_buf[addr];
        clip->xs = xs;
        clip->ys = ys + clip->stup* max_height;
        clip->w  = width;

        if (clip->stup == clip->num-1)
        {
            clip->h = height - (clip->num-1) * max_height; // 最后一个分段可能高度不同
        }
        else
        {
            clip->h = max_height;
        }
    }
    SC_pfb_init(clip,colour);
    // printf("x=%d,y=%d,w=%d,h=%d clip->num=%d clip->stup=%d\r\n",clip->xs,clip->ys,clip->w,clip->h,clip->num,clip->stup);
}

///计算相交区
int SC_pfb_intersection(SC_tile *dest,SC_ARER *p,int xs,int ys,int xe,int ye)
{
    p->ys= SC_MAX(ys,dest->ys);             //ys边界限定
    p->ye= SC_MIN(ye,dest->ys+dest->h-1);   //ye边界限定
    if(p->ys>p->ye) return 0;
    p->xs= SC_MAX(xs,dest->xs);            //xs边界限定
    p->xe= SC_MIN(xe,dest->xs+dest->w-1);  //xe边界限定
    if(p->xs>p->xe) return 0;

    return 1;     //相交

}

// sc_gui.c

SC_GUI  *gui;

uint16_t  SC_pfb_buf[SC_PFB_BUF_SIZE];    //内存池2K

//快速运算alpha算法
uint16_t alphaBlend( uint16_t fc, uint16_t bc,uint8_t alpha)
{
    if(alpha>240)  return fc;
    if(alpha<16)   return bc;
    // Split out and blend 5 bit red and blue channels
    uint32_t rxb = bc & 0xF81F;
    rxb += ((fc & 0xF81F) - rxb) * (alpha >> 2) >> 6;
    // Split out and blend 6 bit green channel
    uint32_t xgx = bc & 0x07E0;
    xgx += ((fc & 0x07E0) - xgx) * alpha >> 8;
    // Recombine channels
    return (rxb & 0xF81F) | (xgx & 0x07E0);
}
//初始化系统注册画点函数
void SC_GUI_Init(uint16_t bkc,uint16_t bc,uint16_t fc)
{
    static SC_GUI tft;
    gui = &tft;
    gui->bkc=bkc;
    gui->bc= bc;
    gui->fc= fc;
    gui->alpha=255;

    gui->dma_i= 0;
    gui->dma_prt= (uint16_t*)SC_pfb_buf;
    gui->pfb_buf= (uint16_t*)SC_pfb_buf;

    gui->lcd_area.xs=0;
    gui->lcd_area.ys=0;
    gui->lcd_area.xe=LCD_SCREEN_WIDTH-1;   //
    gui->lcd_area.ye=LCD_SCREEN_HEIGHT-1;
}

///=============================字体位图================================================
/**fun: get_bpp_value*/
static inline uint16_t get_bpp_value(const uint8_t *buffer,uint16_t offset, uint8_t bpp)
{
    uint16_t alpha=0;
    switch (bpp)
    {
    case 8:
        alpha= (buffer[offset]);
        break;
    case 4:
        alpha= (buffer[offset / 2] >> (4 * (1 - (offset % 2)))) & 0x0F;
        alpha= alpha*17;
        break;
    case 2:
        alpha= (buffer[offset / 4] >> (2 * (3 - (offset % 4)))) & 0x03;
        alpha= alpha*85;
        break;
    case 1:
        alpha= (buffer[offset / 8] >> (7 - (offset % 8))) & 0x01 ? 255 : 0;
        break;
    default:
        break;
    }
    return  alpha;
}

///====================== LVGL字体=====================================
//将utf-8编码转为unicode编码（函数来自LVGL）
uint32_t lv_txt_utf8_next(const char * txt, uint32_t * i)
{
    uint32_t result = 0;
    uint32_t i_tmp = 0;
    if(i == NULL) i = &i_tmp;
    if((txt[*i] & 0x80) == 0)     //Normal ASCII
    {
        result = txt[*i];
        (*i)++;
    }
    else    //Real UTF-8 decode
    {
        // bytes UTF-8 code
        if((txt[*i] & 0xE0) == 0xC0)
        {
            result = (uint32_t)(txt[*i] & 0x1F) << 6;
            (*i)++;
            if((txt[*i] & 0xC0) != 0x80) return 0; //Invalid UTF-8 code
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
        //3 bytes UTF-8 code
        else if((txt[*i] & 0xF0) == 0xE0)
        {
            result = (uint32_t)(txt[*i] & 0x0F) << 12;
            (*i)++;

            if((txt[*i] & 0xC0) != 0x80) return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 6;
            (*i)++;

            if((txt[*i] & 0xC0) != 0x80) return 0;
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
        else if((txt[*i] & 0xF8) == 0xF0)
        {
            result = (uint32_t)(txt[*i] & 0x07) << 18;
            (*i)++;
            if((txt[*i] & 0xC0) != 0x80) return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 12;
            (*i)++;

            if((txt[*i] & 0xC0) != 0x80) return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 6;
            (*i)++;

            if((txt[*i] & 0xC0) != 0x80) return 0;
            result += txt[*i] & 0x3F;
            (*i)++;
        }
        else
        {
            (*i)++;
        }
    }
    return result;
}

//显示一个lvgl字符
void SC_pfb_lv_letter(SC_tile *dest,SC_ARER *align,int xs,int ys,lv_font_glyph_dsc_t *dsc,uint32_t unicode, lv_font_t *font,uint16_t fc,uint16_t bc)
{
    uint16_t w= dsc->adv_w;
    uint16_t h= font->line_height;
    int xe=xs+w-1;
    int ye=ys+h-1;
    int offs_y= ys+(h-dsc->box_h-dsc->ofs_y-font->base_line);  //
    int offs_x= xs+dsc->ofs_x;
    //-----------对齐窗口-------------------------
    ys= SC_MAX(ys,  align->ys);  //ys边界限定
    ye= SC_MIN(ye,  align->ye);  //ye边界限定
    if(ys>=ye) return ;
    xs= SC_MAX(xs, align->xs);    //xs边界限定
    xe= SC_MIN(xe, align->xe);    //xe边界限定
    if(xs>=xe) return ;

    //-----------计算相交-------------------------
    SC_ARER intersection;
    if(!SC_pfb_intersection(dest,&intersection,xs,ys,xe,ye))
    {
        return;
    }
    int x,y,src_x,src_y;
    uint16_t alpha;
    const uint8_t *src =font->get_glyph_bitmap(font,unicode);
    for ( y = intersection.ys; y <=intersection.ye; y++)
    {
        int dest_offs=(y-dest->ys) * dest->w -dest->xs;
        src_y = y- offs_y;
        for ( x = intersection.xs; x <=intersection.xe; x++)
        {
            src_x = x - offs_x;
            if(bc!=gui->bkc)
            {
                set_pixel_value(dest,dest_offs+x,gui->alpha,bc);
            }
            if (src_x < 0 || src_x >= dsc->box_w||src_y < 0 || src_y >= dsc->box_h)
            {
                continue;
            }
            else
            {
                alpha=get_bpp_value(src,src_y * dsc->box_w+src_x, dsc->bpp);
                set_pixel_value(dest,dest_offs+x,alpha,fc);
            }
        }
    }
}


//设置字体间距
void SC_set_font_x_scale(lv_font_t* font,uint16_t Xscale)
{
    lv_font_fmt_txt_dsc_t *dsc=font->dsc;
    dsc->kern_scale=Xscale;
}

//计算长度+间距
static bool lv_get_glyph_dsc(lv_font_t *font, lv_font_glyph_dsc_t *glyph, uint32_t unicode,int *len)
{
    lv_font_fmt_txt_dsc_t *dsc=font->dsc;
    if( font->get_glyph_dsc(font,glyph,unicode,0))
    {
        glyph->adv_w+=dsc->kern_scale;
        *len+=glyph->adv_w;
        return 1;
    }
    return 0;
}
//长文本显示自动换行，支持\n换行，返回0未结束，返回1结束.
int SC_pfb_printf(SC_tile *dest, int x,int y,const char* txt,uint16_t fc,uint16_t bc, lv_font_t* font, int line_space)
{
    lv_font_glyph_dsc_t g;
    uint32_t i =0;
    uint32_t unicode;
    SC_ARER *box=&gui->lcd_area;
    if(dest)
    {
        //===========计算相交===============
        SC_ARER intersection;
        if(!SC_pfb_intersection(dest,&intersection,box->xs,box->ys,box->xe,box->ye))
        {
            return 0;
        }
    }
    int xlen=0;
    for(;; x+=g.adv_w)
    {
        unicode = lv_txt_utf8_next(txt,&i);      //txt转unicode
        if(lv_get_glyph_dsc(font,&g,unicode,&xlen)==0) //长度累加直到结束
        {
            if(unicode==0)                      //结束符
            {
                return 1;
            }
            else if(unicode=='\n'||unicode=='\r')
            {
                x=box->xe;
                continue;
            }
            else
            {
                unicode=' ';               //未知字体
                lv_get_glyph_dsc(font,&g,unicode,&xlen);
            }
        }
        if(x+g.adv_w>=box->xe)        //换行
        {
            x=box->xs;
            y+=font->line_height+line_space;
            if(y>box->ye)  break;
        }
        SC_pfb_lv_letter(dest,box,x,y,&g,unicode,font,fc,bc);
    }
    return 0;
}

// sc_arc.c

static const int16_t sin0_90_table[] =
{
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32767
};

/*******************************************
 * Return with sinus of an angle
 * @param angle
 * @return sinus of 'angle'. sin(-90) = -32767, sin(90) = 32767
 */
int16_t sc_sin(int16_t angle)
{
    int16_t ret = 0;
    angle       = angle % 360;
    if(angle < 0) angle = 360 + angle;
    if(angle < 90)
    {
        ret = sin0_90_table[angle];
    }
    else if(angle >= 90 && angle < 180)
    {
        angle = 180 - angle;
        ret   = sin0_90_table[angle];
    }
    else if(angle >= 180 && angle < 270)
    {
        angle = angle - 180;
        ret   = -sin0_90_table[angle];
    }
    else     /*angle >=270*/
    {
        angle = 360 - angle;
        ret   = -sin0_90_table[angle];
    }
    return ret;
}
int16_t sc_cos(int16_t angle)
{
    return -sc_sin(angle-90);
}
// Compute the fixed point square root of an integer and
// return the 8 MS bits of fractional part.
// Quicker than sqrt() for processors that do not have an FPU (e.g. RP2040)
uint8_t sc_sqrt(uint32_t num)
{
    if (num > (0x40000000)) return 0;
    uint32_t bsh = 0x00004000;
    uint32_t fpr = 0;
    uint32_t osh = 0;
    uint32_t bod;
    // Auto adjust from U8:8 up to U15:16
    while (num>bsh)
    {
        bsh <<= 2;
        osh++;
    }
    do
    {
        bod = bsh + fpr;
        if(num >= bod)
        {
            num -= bod;
            fpr = bsh + bod;
        }
        num <<= 1;
    }
    while(bsh >>= 1);
    return fpr>>osh;
}

/**fun: pfb圆角矩形*/
void SC_pfb_RoundFrame(SC_tile *dest,int xs,int ys,int xe,int ye, int r,int ir, uint16_t ac,uint16_t bc)
{
    int r1 =   ir*ir;
    int r2 =   r*r;
    int rmax = (r+1)*(r+1);
    int rmin=  ir>0? (ir-1)*(ir-1):0;
    int ax=xs+r,ay=ys+r;
    int bx=xe-r,by=ye-r;
    int w=bx-ax, h=by-ay;
    SC_ARER intersection;
    if(!SC_pfb_intersection(dest,&intersection,xs,ys,xe,ye))
    {
        return ;
    }
    int dy2,temp;
    int x,y,cx,cy;
    uint16_t alpha;
    for ( y = intersection.ys,cy=ay; y<=intersection.ye; y++)
    {
        if(y>gui->lcd_area.ye) {
            break;
        }
        int dest_offs=(y-dest->ys) * dest->w-dest->xs;
        if(h>0&&y>ay)
        {
            cy=by;
            if(y<cy)      //(y>ay&&y<cy)       //中间填充
            {
                for ( x = intersection.xs; x <=intersection.xe; x++)
                {
                    if(x>gui->lcd_area.xe) {
                        break;
                    }
                    if(x<=ax-ir||x>=bx+ir)     //左右垂直线，线宽判断
                    {
                        set_pixel_value(dest,dest_offs+x,gui->alpha,ac);
                    }
                    else
                    {
                        if(bc!=gui->bkc)
                        {
                            set_pixel_value(dest,dest_offs+x,gui->alpha,bc);
                        }

                    }
                }
                continue;
            }
        }
        dy2 = (y-cy)*(y-cy);
        for ( x = intersection.xs,cx=ax; x <=intersection.xe; x++)
        {
            if(x>gui->lcd_area.xe) {
                break;
            }
            if(w>0&&x>ax)
            {
                cx=bx;
                if(x<cx)                    //(x>ax&&x<bx)
                {
                    if(dy2>=r1)             //上下水平直线，线宽判断
                    {
                        set_pixel_value(dest,dest_offs+x,gui->alpha,ac);
                    }
                    else
                    {
                        if(bc!=gui->bkc)
                        {
                            set_pixel_value(dest,dest_offs+x,gui->alpha,bc);
                        }

                    }
                    continue;
                }
            }
            temp =(x-cx)*(x-cx) + dy2;
            if (temp>=rmax)
            {
                if(x>cx) break;
                continue;                       //大于rmax外圆跳过
            }
            if (temp<r1)
            {
                if(bc!=gui->bkc)
                {
                    set_pixel_value(dest,dest_offs+x,gui->alpha,bc);
                }
                if (temp>rmin)
                {
                    alpha= sc_sqrt(temp);       //内边缘抗锯齿
                    set_pixel_value(dest,dest_offs+x,alpha*gui->alpha/255,ac);
                }
                continue;
            }
            else if (temp> r2)
            {
                alpha=255-sc_sqrt(temp);        //外边缘抗锯齿
                set_pixel_value(dest,dest_offs+x,alpha*gui->alpha/255,ac);
            }
            else
            {
                alpha=255;
                set_pixel_value(dest,dest_offs+x,gui->alpha,ac);
            }
        }
    }
}
