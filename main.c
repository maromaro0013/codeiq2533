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
  char w;
  char h;
  char x;
  char y;
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

  char tile_buff[cFIELD_TILE_BUFF_SIZE];

  TILE tiles[cFIELD_SIZE*cFIELD_SIZE];
}FIELD;

FIELD_INFO g_filed_info;
//char g_tile_limit_dp[cFIELD_TILE_HASH_MAX + 1];

static const TILE g_tile_patterns[cTILE_PATTERNS_MAX] = {
  {1, 1, 0, 0},
  {2, 2, 0, 0},
  {4, 2, 0, 0},
  {4, 4, 0, 0},
};

int chk_tile_limit(TILE* p) {
  FIELD_INFO* info = &g_filed_info;

  if (p->y + p->h > info->h) {
    return FALSE;
  }
  if (p->x + p->w > info->w) {
    return FALSE;
  }
  return TRUE;
}
/*
unsigned short create_tile_hash(TILE* p) {
  unsigned short ret;
  ret = ((p->width - 1) << 10 ) | ((p->height -1) << 8) | (p->x << 4) | p->y;
  p->tile_hash = ret;

  return ret;
}
*/
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

  if (chk_tile_limit(p) == FALSE) {
    return FALSE;
  }

  for (y = p->y; y < p->y + p->h; y++) {
    for (x = p->x; x < p->x + p->w; x++) {
      if (get_tile_buff(f->tile_buff, y, x)) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

int tile_placement(FIELD* f, TILE* p) {
  int x, y;

  f->tiles[f->tiles_count] = *p;

  f->tiles_count++;
  f->size_amount += p->w*p->h;

  for (y = p->y; y < (p->y + p->h); y++) {
    for (x = p->x; x < (p->x + p->w); x++) {
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

  tile.w = g_tile_patterns[pattern].w;
  tile.h = g_tile_patterns[pattern].h;

  if (pattern == 0) {
    next_field = *f;
    for (tile.y = 0; tile.y < info->h; tile.y++) {
      for (tile.x = 0; tile.x < info->w; tile.x++) {
        if (!get_tile_buff(next_field.tile_buff, tile.y, tile.x)) {
          tile_placement(&next_field, &tile);
          if (chk_fill_field(&next_field) == TRUE) {
            for (i = 0; i < next_field.tiles_count; i++) {
              printf("%d*%d(%d,%d), ", next_field.tiles[i].w, next_field.tiles[i].h, next_field.tiles[i].x, next_field.tiles[i].y);
            }
            printf("\n");
            return 1; // 終着点
          }
        }
      }
    }
    printf("error!!\n");
    return 0; // ここに来ることはないはず(必ず"chk_fill_field"に引っかかるはず)
  }
  else {
    for (tile.y = 0; tile.y < info->h; tile.y++) {
      for (tile.x = 0; tile.x < info->w; tile.x++) {
        if (!get_tile_buff(f->tile_buff, tile.y, tile.x)) {
          next_field = *f;
          if (chk_tile_placement(&next_field, &tile)) {
            tile_placement(&next_field, &tile);
            if (chk_fill_field(&next_field) == TRUE) {
              for (i = 0; i < next_field.tiles_count; i++) {
                printf("%d*%d(%d,%d), ", next_field.tiles[i].w, next_field.tiles[i].h, next_field.tiles[i].x, next_field.tiles[i].y);
              }
              printf("\n");
              ret += 1;
            }
            else {
              if (next_field.size_amount >= tile.w*tile.h) {
                ret += solve_field(&next_field, pattern);
              }
              //ret += solve_field(&next_field, pattern - 1);
            }
          }
        }
      }
    }
    ret += solve_field(f, pattern - 1);
  }
  return ret;
/*
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

  memset((void*)&field, 0, sizeof(field));
  ret = solve_field(&field, cTILE_PATTERNS_MAX - 1);
  printf("%d\n", ret);

  return 0;
}
