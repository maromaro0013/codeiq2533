#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE (0)
#define TRUE  (1)

#define cTILE_PATTERNS_MAX (4)
#define cFIELD_TILES_MAX   (128)

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
  int tiles_count;

  TILE tiles[cFIELD_TILES_MAX];
}FIELD;

FIELD_INFO g_filed_info;

void copy_field(FIELD* dest, FIELD* src) {
  memcpy((void*)dest->tiles, (void*)src->tiles, sizeof(TILE)*src->tiles_count);
  dest->tiles_count = src->tiles_count;
}

int chk_tile_limit(TILE* p) {
  FIELD_INFO* info = &g_filed_info;

  if (p->y < 0) {
    return FALSE;
  }
  if (p->y + p->height > info->height) {
    return FALSE;
  }
  if (p->x < 0) {
    return FALSE;
  }
  if (p->x + p->width > info->width) {
    return FALSE;
  }
  return TRUE;
}

void init_tile_patterns(TILE* tile_patterns) {
  tile_patterns[0].width = tile_patterns[0].height = 1;
  tile_patterns[1].width = tile_patterns[1].height = 2;
  tile_patterns[2].width = 4;
  tile_patterns[2].height = 2;
  tile_patterns[3].width = tile_patterns[3].height = 4;
}

int chk_tile_exist(FIELD* f, char x, char y) {
  int i = 0;
  TILE* p = NULL;

  for (i = 0; i < f->tiles_count; i++) {
    p = &f->tiles[i];
    if (x >= p->x && x < p->x + p->width) {
      if (y >= p->y && y < p->y + p->height) {
        return i;
      }
    }
  }
  return -1;
}

int tile_collision(TILE* p0, TILE* p1) {
  int x0 = p0->x;
  int x1 = p0->x + p0->width;
  int x2 = p1->x;
  int x3 = p1->x + p1->width;

  int y0 = p0->y;
  int y1 = p0->y + p0->height;
  int y2 = p1->y;
  int y3 = p1->y + p1->height;

  if (x0 >= x2 && x0 < x3) {
    if (y0 >= y2 && y0 < y3) {
      return TRUE;
    }
    if (y1 > y2 && y1 <= y3) {
      return TRUE;
    }
  }
  if (x1 > x2 && x1 <= x3) {
    if (y1 > y2 && y1 <= y3) {
      return TRUE;
    }
    if (y0 >= y2 && y0 < y3) {
      return TRUE;
    }
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

    if (tile_collision(p, target)) {
      return FALSE;
    }
  }

  return TRUE;
}

int tile_placement(FIELD* f, TILE* p) {
  TILE* target = &f->tiles[f->tiles_count];

  target->width = p->width;
  target->height = p->height;
  target->x = p->x;
  target->y = p->y;
  f->tiles_count++;

  return TRUE;
}

int solve_field(FIELD* f, TILE* patterns, char x, char y) {
  int i = 0;
  int j = 0;
  int ret = 0;
  TILE tile;
  FIELD* next_field = NULL;
  FIELD_INFO* info = &g_filed_info;
  int exist_idx = -1;

  exist_idx = chk_tile_exist(f, x, y);
  if (exist_idx > 0) {
    x += f->tiles[exist_idx].width;
  }

  for (i = 0; i < cTILE_PATTERNS_MAX; i++) {
    tile = patterns[i];
    tile.x = x;
    tile.y = y;
    if (chk_tile_placement(f, &tile)) {
      if (x + tile.width >= info->width && y + tile.height >= info->height) {
        for (j = 0; j < f->tiles_count; j++) {
          printf("%d,", f->tiles[j].width);
        }
        printf("%d\n", tile.width);
        ret += 1;
      }
      else {
        int next_x = x + tile.width;
        int next_y = y;
        if (next_x >= info->width) {
          next_x = 0;
          next_y += 1;
        }

        next_field = (FIELD*)malloc(sizeof(FIELD));
        copy_field(next_field, f);
        tile_placement(next_field, &tile);

        ret += solve_field(next_field, patterns, next_x, next_y);
        free(next_field);
      }
    }
  }
  return ret;
}

int main (int argc, char** argv) {
  char buff[256];
  char tmpbuff[256];
  TILE tile_patterns[cTILE_PATTERNS_MAX];
  FIELD field;
  int ret = 0;

  init_tile_patterns(tile_patterns);

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

  //printf("w:%d - h:%d\n", g_filed_info.width, g_filed_info.height);

  field.tiles_count = 0;
  ret = solve_field(&field, tile_patterns, 0, 0);
  printf("ret:%d\n", ret);
//  printf("size:%d\n", sizeof(FIELD));

  return 0;
}
