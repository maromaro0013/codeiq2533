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
  memcpy((void*)dest->tiles, (void*)src->tiles, sizeof(dest->tiles));
  dest->tiles_count = src->tiles_count;
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

int chk_fill_field(FIELD* f) {
  int amt = 0;
  int i = 0;
  int j = 0;
  FIELD_INFO* info = &g_filed_info;
  int max = info->width*info->height;

  for (i = 0; i < f->tiles_count; i++) {
    amt += f->tiles[i].width*f->tiles[i].height;
    if (amt > max) {
      return -1;
    }
    else if (amt >= max) {
      return TRUE;
    }
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
  TILE* target = &f->tiles[f->tiles_count];

  target->width = p->width;
  target->height = p->height;
  target->x = p->x;
  target->y = p->y;
  f->tiles_count++;

  return TRUE;
}

int solve_field(FIELD* f, TILE* patterns) {
  int i = 0;
  int j = 0;
  int pattern = 0;
  int ret = 0;
  TILE tile;
  FIELD* next_field = NULL;
  FIELD_INFO* info = &g_filed_info;
  int idx = 0;
  int z = 0;
  int chk = 0;

  for (i = 0; i < info->width; i++) {
    for (j = 0; j < info->height; j++) {
      idx = chk_tile_exist(f, i, j);
      if (idx == -1) {
        break;
      }
    }
    if (idx == -1) {
      break;
    }
  }
  if (idx != -1) {
    return 0;
  }

  tile.x = i;
  tile.y = j;
  for (pattern = 0; pattern < cTILE_PATTERNS_MAX; pattern++) {
    tile.width = patterns[pattern].width;
    tile.height = patterns[pattern].height;

    if (chk_tile_placement(f, &tile)) {
      next_field = (FIELD*)malloc(sizeof(FIELD));
      copy_field(next_field, f);
      tile_placement(next_field, &tile);
      chk = chk_fill_field(next_field);
      if (chk == TRUE) {
        ret += 1;
      }
      else if (chk != -1) {
        ret += solve_field(next_field, patterns);
      }
      free(next_field);
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

  field.tiles_count = 0;
  ret = solve_field(&field, tile_patterns);
  printf("%d\n", ret);

  return 0;
}
