#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE (0)
#define TRUE  (1)

#define cTILE_PATTERNS_MAX    (4)
#define cFIELD_SIZE           (10)
#define cFIELD_TILE_BUFF_SIZE (13)        // 104bits

#define cFIELD_TILE_HASH_MAX  (4*4*16*16) // width(2bits)*height(2bits)*x(4bits)*y(4bits)

#define get_tile_buff(buff, y, x)      ( (buff[(y*cFIELD_SIZE + x)/8] >> (x%8)) & 1 )
#define set_tile_buff(buff, y, x)      ( (buff[(y*cFIELD_SIZE + x)/8] |= 1 << (x%8)) )

typedef struct TILE_t {
  char width;
  char height;
  char x;
  char y;

  unsigned short tile_hash;
}TILE;

typedef struct FIELD_INFO_t {
  char w;
  char h;

  char padd[2];
}FIELD_INFO;

typedef struct FIELD_t {
  char tiles_count;
  char size_amount;
  char buff;

  TILE tiles[cFIELD_SIZE*cFIELD_SIZE];
  char tile_buff[cFIELD_SIZE*cFIELD_SIZE];
}FIELD;

FIELD_INFO g_filed_info;
char g_tile_limit_dp[cFIELD_TILE_HASH_MAX + 1];

static const TILE g_tile_patterns[cTILE_PATTERNS_MAX] = {
  {1, 1, 0, 0},
  {2, 2, 0, 0},
  {4, 2, 0, 0},
  {4, 4, 0, 0},
};

int chk_tile_limit(TILE* p) {
  FIELD_INFO* info = &g_filed_info;

  if (p->y + p->height > info->h) {
    return FALSE;
  }
  if (p->x + p->width > info->w) {
    return FALSE;
  }
  return TRUE;
}

unsigned short create_tile_hash(TILE* p) {
  unsigned short ret;
  ret = ((p->width - 1) << 10 ) | ((p->height -1) << 8) | (p->x << 4) | p->y;
  p->tile_hash = ret;

  return ret;
}

int chk_fill_field(FIELD* f) {
  FIELD_INFO* info = &g_filed_info;

  if (f->size_amount >= info->w*info->h) {
    return TRUE;
  }
  return FALSE;

}

int chk_tile_placement(const FIELD* f, TILE* p) {
  FIELD_INFO* info = &g_filed_info;
  int i = 0;
  int x, y;
  TILE* target = NULL;

  if (g_tile_limit_dp[p->tile_hash] == -1) {
    g_tile_limit_dp[p->tile_hash] = chk_tile_limit(p);
  }
  if (g_tile_limit_dp[p->tile_hash] == FALSE) {
    return FALSE;
  }

  for (y = p->y; y < p->y + p->height; y++) {
    for (x = p->x; x < p->x + p->width; x++) {
      if (get_tile_buff(f->tile_buff, y, x)) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

int tile_placement(FIELD* f, TILE* p) {
  int x, y;

  f->tiles[f->tiles_count].width = p->width;
  f->tiles[f->tiles_count].height = p->height;
  f->tiles[f->tiles_count].x = p->x;
  f->tiles[f->tiles_count].y = p->y;

  f->tiles_count++;
  f->size_amount += p->width*p->height;

  for (y = p->y; y < (p->y + p->height); y++) {
    for (x = p->x; x < (p->x + p->width); x++) {
      set_tile_buff(f->tile_buff, y, x);
    }
  }
  return TRUE;
}

int solve_field(const FIELD* f, int pattern) {
  int ret = 0;
  int i = 0;
  TILE tile;
  FIELD next_field;
  FIELD_INFO* info = &g_filed_info;

  tile.width = g_tile_patterns[pattern].width;
  tile.height = g_tile_patterns[pattern].height;

  for (tile.y = 0; tile.y < info->h; tile.y++) {
    for (tile.x = 0; tile.x < info->w; tile.x++) {
      if (!get_tile_buff(f->tile_buff, tile.y, tile.x)) {
        create_tile_hash(&tile);
        if (pattern == 0 || chk_tile_placement(f, &tile)) {
          next_field = *f;
          tile_placement(&next_field, &tile);

          if (chk_fill_field(&next_field) == TRUE) {
            for (i = 0; i < next_field.tiles_count; i++) {
              printf("%d*%d(%d,%d), ", next_field.tiles[i].width, next_field.tiles[i].height, next_field.tiles[i].x, next_field.tiles[i].y);
            }
            printf("\n");
            if (pattern == 0) {
              return 1;
            }
            else {
              ret += solve_field(f, pattern - 1);
            }
          }
          else {
            return solve_field(&next_field, pattern);
          }
        }
      }
    }
  }

  ret += solve_field(f, pattern - 1);

  return ret;
/*
int solve_field(FIELD* f) {
  int pattern = 0;
  int ret = 0;
  TILE tile;
  FIELD next_field;
  FIELD_INFO* info = &g_filed_info;

  for (tile.y = 0; tile.y < info->h; tile.y++) {
    for (tile.x = 0; tile.x < info->w; tile.x++) {
//      if (f->tile_buff[tile.y*info->width + tile.x] != TRUE) {
      if (!get_tile_buff(f->tile_buff, tile.y, tile.x)) {
        goto exit_loop;
      }
    }
  }

exit_loop:
  for (pattern = 0; pattern < cTILE_PATTERNS_MAX; pattern++) {
    tile.width = g_tile_patterns[pattern].width;
    tile.height = g_tile_patterns[pattern].height;
    create_tile_hash(&tile);

    if (pattern == 0 || chk_tile_placement(f, &tile)) {
      next_field = *f;
      tile_placement(&next_field, &tile);

      if (chk_fill_field(&next_field) == TRUE) {
        ret += 1;
      }
      else {
        ret += solve_field(&next_field);
      }
    }
  }
  return ret;
*/
}


int main (int argc, char** argv) {
  char buff[256];
  char tmpbuff[256];
  FIELD field;
  int ret = 0;

  fgets(buff, sizeof(buff), stdin);

  int i = 0;
  int j = 0;
  for (i = 0; i < strlen(buff); i++){
    if (buff[i] == ',') {
      tmpbuff[i] = '\0';
      break;
    }
    tmpbuff[i] = buff[i];
  }
  g_filed_info.w = atoi(tmpbuff);
  i += 1;

  for (j = 0; i < strlen(buff); j++, i++){
    if (buff[i] == ',') {
      tmpbuff[j] = '\0';
      break;
    }
    tmpbuff[j] = buff[i];
  }
  g_filed_info.h = atoi(tmpbuff);

  memset((void*)g_tile_limit_dp, -1, sizeof(g_tile_limit_dp));

  memset((void*)&field, 0, sizeof(field));
  ret = solve_field(&field, cTILE_PATTERNS_MAX - 1);
  printf("%d\n", ret);

  return 0;
}
