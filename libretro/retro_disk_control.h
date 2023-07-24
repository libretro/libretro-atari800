/****************************************************************************
 *  Caprice32 libretro port
 *
 *  Copyright not6 - r-type (2015-2018)
 *  Copyright David Colmenero - D_Skywalk (2019-2021)
 *  Copyright Daniel De Matteis (2012-2021)
 *
 *  Redistribution and use of this code or any derivative works are permitted
 *  provided that the following conditions are met:
 *
 *   - Redistributions may not be sold, nor may they be used in a commercial
 *     product or activity.
 *
 *   - Redistributions that are modified from the original source must include the
 *     complete source code, including the source code for all components used by a
 *     binary built from the modified sources. However, as a special exception, the
 *     source code distributed need not include anything that is normally distributed
 *     (in either source or binary form) with the major components (compiler, kernel,
 *     and so on) of the operating system on which the executable runs, unless that
 *     component itself accompanies the executable.
 *
 *   - Redistributions must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************/

#ifndef RETRO_DISK_CONTROL_H__
#define RETRO_DISK_CONTROL_H__

#include <stdbool.h>

//*****************************************************************************
// Disk control structure and functions
#define DC_MAX_SIZE 20

//static INLINE bool string_is_empty(const char* data)
//{
//	return !data || (*data == '\0');
//}

enum dc_image_type {
	DC_IMAGE_TYPE_NONE = 0,
	DC_IMAGE_TYPE_FLOPPY,
	DC_IMAGE_TYPE_TAPE,
	DC_IMAGE_TYPE_MEM,
	DC_IMAGE_TYPE_UNKNOWN
};

struct dc_storage{
	char* command;
	char* files[DC_MAX_SIZE];
	char* names[DC_MAX_SIZE];
	enum dc_image_type types[DC_MAX_SIZE];
	unsigned unit;
	unsigned count;
	int index;
	bool eject_state;
	bool replace;
	unsigned index_prev;
};

typedef struct dc_storage dc_storage;
dc_storage* dc_create(void);

void dc_parse_m3u(dc_storage* dc, const char* m3u_file);
bool dc_add_file(dc_storage* dc, const char* filename);
void dc_free(dc_storage* dc);
void dc_reset(dc_storage* dc);
int dc_replace_file(dc_storage* dc, int index, const char* filename);
bool dc_remove_file(dc_storage* dc, int index);
enum dc_image_type dc_get_image_type(const char* filename);

#endif
