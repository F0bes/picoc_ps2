#include "../../interpreter.h"

#include <draw.h>
#include <graph.h>
#include <packet.h>
#include <gs_psm.h>
#include <dma.h>

framebuffer_t fb;

void PS2_DrawSetup()
{
	graph_vram_clear();
	fb.address = graph_vram_allocate(640, 448, GS_PSM_24, GRAPH_ALIGN_PAGE);
	fb.psm = GS_PSM_24;
	fb.width = 640;
	fb.height = 448;
	
	graph_initialize(fb.address,fb.width,fb.height,fb.psm,0,0);
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF); // Wait for any possible transfers going on

	packet_t* packet = packet_init(30, PACKET_NORMAL);
	qword_t* q = packet->data;

	zbuffer_t zb;
	zb.enable = 0;
	zb.address = 0;
	zb.mask = 0;
	zb.method = 0;
	zb.zsm = 0;
	
	q = draw_setup_environment(q, 0, &fb, &zb);
	q = draw_disable_tests(q, 0, &zb);
	q = draw_primitive_xyoffset(q,0,0,0);
	q = draw_finish(q);

	dma_channel_send_normal(DMA_CHANNEL_GIF, packet->data, q - packet->data, 0, 0);
	packet_free(packet);
}

void PS2_DrawRect(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{

	int x1 = Param[0]->Val->Integer;
	int y1 = Param[1]->Val->Integer;
	int x2 = Param[2]->Val->Integer;
	int y2 = Param[3]->Val->Integer;
	int r = Param[4]->Val->Integer;
	int g = Param[5]->Val->Integer;
	int b = Param[6]->Val->Integer;
	
	
	packet_t* packet = packet_init(30, PACKET_NORMAL);
	qword_t* q = packet->data;
	PACK_GIFTAG(q, GIF_SET_TAG(1, 1, GIF_PRE_ENABLE, GIF_SET_PRIM(GIF_PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0), GIF_FLG_PACKED, 3),
	GIF_REG_RGBAQ | (GIF_REG_XYZ2 << 4) | (GIF_REG_XYZ2 << 8));
	q++;
	q->dw[0] = (u64)(r) | ((u64)g << 32);
	q->dw[1] = (u64)(b) | ((u64)1 << 32);
	q++;
	// XYZ2
	q->dw[0] = (u64)((((x1 << 4)) | (((u64)(y1 << 4)) << 32)));
	q->dw[1] = (u64)(0);
	q++;
	// XYZ2
	q->dw[0] = (u64)((((x2 << 4)) | (((u64)(y2 << 4)) << 32)));
	q->dw[1] = (u64)(0);
	q++;
	q = draw_finish(q);
	dma_channel_send_normal(DMA_CHANNEL_GIF,  packet->data, q - packet->data, 0, 0);
	draw_wait_finish();
	packet_free(packet);
	graph_wait_vsync();
	ReturnValue->Val->Integer = 0;
}


struct LibraryFunction PS2DrawFunctions[] =
	{
		{PS2_DrawRect, "int drawrect(int, int, int, int, int, int, int);"},
		{NULL, NULL}};


void PS2DRAWLibraryInit(Picoc* pc)
{
	printf("PS2DRAWLibraryInit\n");
	IncludeRegister(pc, "draw.h", &PS2_DrawSetup, &PS2DrawFunctions[0], NULL);
}