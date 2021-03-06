// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  sdlsync_mini.c - Minimal core synchronization functions
//
//============================================================

#include "osdcore.h"
#include "osdsync.h"

struct _osd_event
{
	void *  ptr;
};

struct _osd_thread {
	void *  ptr;
};


//============================================================
//  osd_event_alloc
//============================================================

osd_event *osd_event_alloc(int manualreset, int initialstate)
{
	return nullptr;
}


//============================================================
//  osd_event_free
//============================================================

void osd_event_free(osd_event *event)
{
}


//============================================================
//  osd_event_set
//============================================================

void osd_event_set(osd_event *event)
{
}


//============================================================
//  osd_event_reset
//============================================================

void osd_event_reset(osd_event *event)
{
}


//============================================================
//  osd_event_wait
//============================================================

int osd_event_wait(osd_event *event, osd_ticks_t timeout)
{
	return TRUE;
}


//============================================================
//  osd_thread_create
//============================================================

osd_thread *osd_thread_create(osd_thread_callback callback, void *cbparam)
{
	return nullptr;
}


//============================================================
//  osd_thread_adjust_priority
//============================================================

int osd_thread_adjust_priority(osd_thread *thread, int adjust)
{
	return FALSE;
}


//============================================================
//  osd_thread_cpu_affinity
//============================================================

int osd_thread_cpu_affinity(osd_thread *thread, UINT32 mask)
{
	return TRUE;
}


//============================================================
//  osd_thread_wait_free
//============================================================

void osd_thread_wait_free(osd_thread *thread)
{
}
