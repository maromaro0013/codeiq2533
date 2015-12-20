#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE (0)
#define TRUE  (1)

#define cTILE_PATTERNS_MAX    (4)
#define cTILE_SIZE_MAX        (8)
#define cFIELD_FIELD_SIZE_MAX (10)
#define cFIELD_TILES_MAX      (cFIELD_FIELD_SIZE_MAX*cFIELD_FIELD_SIZE_MAX)

typedef struct TILE_t {
  char width;
  char height;
  char x;
  char y;
}TILE;

typedef struct FIELD_INFO_t {
  char width;
  char height;

  char padd[2];
}FIELD_INFO;

typedef struct FIELD_t {
  short tiles_count;
  short size_amount;

  TILE tiles[cFIELD_TILES_MAX];
  char tile_buff[cFIELD_FIELD_SIZE_MAX][cFIELD_FIELD_SIZE_MAX];
}FIELD;

FIELD_INFO g_filed_info;

static const TILE g_tile_patterns[cTILE_PATTERNS_MAX] = {
  {1, 1, 0, 0},
  {2, 2, 0, 0},
  {4, 2, 0, 0},
  {4, 4, 0, 0},
};

void copy_field(FIELD* dest, FIELD* src) {
  dest->tiles_count = src->tiles_count;
  dest->size_amount = src->size_amount;

  memcpy((void*)dest->tiles, (void*)src->tiles, sizeof(TILE)*src->tiles_count);
  memcpy((void*)&dest->tile_buff[0][0], (void*)&src->tile_buff[0][0], sizeof(dest->tile_buff));
}

int chk_tile_limit(TILE* p) {
  FIELD_INFO* info = &g_filed_info;

  if (p->y + p->height > info->height) {
    return FALSE;
  }
  if (p->x + p->width > info->width) {
    return FALSE;
  }
  return TRUE;
}

int chk_fill_field(FIELD* f) {
  FIELD_INFO* info = &g_filed_info;

  if (f->size_amount >= info->width*info->height) {
    return TRUE;
  }
  return FALSE;
}

int tile_collision(TILE* p0, TILE* p1) {
  if ( (p0->x              < p1->x + p1->width ) &&
       (p0->x + p0->width  > p1->x             ) &&
       (p0->y              < p1->y + p1->height) &&
       (p0->y + p0->height > p1->y             )) {
    return TRUE;
  }

  return FALSE;
}

int chk_tile_placement(FIELD* f, TILE* p) {
  int i = 0;
  TILE* target = NULL;

  if (chk_tile_limit(p) == FALSE) {
    return FALSE;
  }

  for (i = 0; i < f->tiles_count; i++) {
    target = &f->tiles[i];

    if (tile_collision(p, target) == TRUE) {
      return FALSE;
    }
  }

  return TRUE;
}

int tile_placement(FIELD* f, TILE* p) {
  FIELD_INFO* info = &g_filed_info;
  TILE* target = &f->tiles[f->tiles_count];
  int i, j;

  target->width = p->width;
  target->height = p->height;
  target->x = p->x;
  target->y = p->y;

  f->tiles_count++;
  f->size_amount += target->width*target->height;

  for (i = p->y; i < (p->y + p->height); i++) {
    for (j = p->x; j < (p->x + p->width); j++) {
      f->tile_buff[i][j] = TRUE;
    }
  }

  return TRUE;
}

int solve_field(FIELD* f) {
  int pattern = 0;
  int ret = 0;
  TILE tile;
  FIELD next_field;
  FIELD_INFO* info = &g_filed_info;
  int flg = FALSE;

  for (tile.y = 0; tile.y < info->height; tile.y++) {
    for (tile.x = 0; tile.x < info->width; tile.x++) {
      if (f->tile_buff[tile.y][tile.x] != TRUE) {
        flg = TRUE;
        break;
      }
    }
    if (flg == TRUE) {
      break;
    }
  }

  for (pattern = 0; pattern < cTILE_PATTERNS_MAX; pattern++) {
    tile.width = g_tile_patterns[pattern].width;
    tile.height = g_tile_patterns[pattern].height;

    if (chk_tile_placement(f, &tile)) {
      copy_field(&next_field, f);
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
  g_filed_info.width = atoi(tmpbuff);
  i += 1;

  for (j = 0; i < strlen(buff); j++, i++){
    if (buff[i] == ',') {
      tmpbuff[j] = '\0';
      break;
    }
    tmpbuff[j] = buff[i];
  }
  g_filed_info.height = atoi(tmpbuff);

  memset((void*)&field, 0, sizeof(field));
  ret = solve_field(&field);
  printf("%d\n", ret);

  return 0;
}
