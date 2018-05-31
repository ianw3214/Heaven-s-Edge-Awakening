#pragma once

#define DEFAULT_TILE_SIZE	64
#define DEFAULT_MAP_WIDTH	40
#define DEFAULT_MAP_HEIGHT	40
#define PAN_SPEED			300

#define BASE_DIR			"../assets"
#define DEFAULT_FONT		"../assets/fonts/Munro.ttf"

#define DEFAULT_ASSETS		"../assets/"
#define DEFAULT_TILEMAP		"../assets/tilemap.png"
#define DEFAULT_BACKGROUND	"../assets/parallax_test.png"

#define DEFAULT_MAP_FOLDER	"../assets/maps/"
#define DEFAULT_MAP_NAME	"default.txt"
#define DEFAULT_MAP_FILE	"../assets/maps/default.txt"

#define DEFAULT_PALETTE_X	0
#define DEFAULT_PALETTE_Y	0

#define TILEMAP				"tilemap"
#define TILE_SELECT			"tile_select"
#define TILE_SELECT_IMG		"../assets/editor/tile_select.png"
#define COLLISION			"collision"
#define COLLISION_IMG		"../assets/editor/collision.png"
#define WHITE_BAR			"white_bar"
#define WHITE_BAR_IMG		"../assets/editor/white_bar.png"

#define BORDER_HOR			"border_horizontal"
#define BORDER_HOR_IMG		"../assets/editor/border_horizontal.png"
#define BORDER_VER			"border_vertical"
#define BORDER_VER_IMG		"../assets/editor/border_vertical.png"
#define BORDER_CORNER		"border_corner"
#define BORDER_CORNER_IMG	"../assets/editor/border_corner.png"

#define PALETTE_BASE		"palette_base"
#define PALETTE_BASE_IMG	"../assets/editor/palette_base.png"
#define PALETTE_SELECT		"palette_select"
#define PALETTE_SELECT_IMG	"../assets/editor/palette_select.png"

// ENTITY SPRITE DEFINITIONS
#define PLAYER				"player"
#define PLAYER_IMG			"../assets/editor/player.png"
#define ENEMY				"enemy"
#define ENEMY_IMG			"../assets/editor/enemy.png"
#define PORTAL				"portal"
#define PORTAL_IMG			"../assets/editor/portal.png"

// HUD SPRITE DEFINITIONS
#define ICON_TILE			"edit_icon"
#define ICON_TILE_IMG		"../assets/editor/tile_icon_normal.png"
#define SELECT_TILE			"tile_select_icon"
#define SELECT_TILE_IMG		"../assets/editor/tile_icon_select.png"
#define ICON_COLLISION		"collision_icon"
#define ICON_COLLISION_IMG	"../assets/editor/collision_icon_normal.png"
#define SELECT_COLLISION	"collision_select_icon"
#define SELECT_COLL_IMG		"../assets/editor/collision_icon_select.png"
#define ICON_ENTITY			"entity_icon"
#define ICON_ENTITY_IMG		"../assets/editor/entity_icon_normal.png"
#define SELECT_ENTITY		"entity_select_icon"
#define SELECT_ENTITY_IMG	"../assets/editor/entity_icon_select.png"

#define PLA_ENTITY_ICON		"pla_ent"
#define PLA_ENTITY_ICON_IMG	"../assets/editor/player_entity_icon.png"
#define PLA_ENTITY_SEL		"pla_ent_sel"
#define PLA_ENTITY_SEL_IMG	"../assets/editor/player_entity_icon_select.png"
#define ENE_ENTITY_ICON		"ene_ent"
#define ENE_ENTITY_ICON_IMG "../assets/editor/enemy_entity_icon.png"
#define ENE_ENTITY_SEL		"ene_ent_sel"
#define ENE_ENTITY_SEL_IMG	"../assets/editor/enemy_entity_icon_select.png"
#define POR_ENTITY_ICON		"spa_ent"
#define POR_ENTITY_ICON_IMG	"../assets/editor/portal_entity_icon.png"
#define POR_ENTITY_SEL		"spa_ent_sel"
#define POR_ENTITY_SEL_IMG	"../assets/editor/portal_entity_icon_select.png"

// MISC SPRITE DEFINITIONS
#define SAVEMAP				"savemap"
#define SAVEMAP_IMG			"../assets/editor/savemap_bg.png"
#define SPAWN_BLOCK			"spawn_block"
#define SPAWN_BLOCK_IMG		"../assets/editor/spawn_block.png"
#define SPAWN_SELECT		"spawn_select"
#define SPAWN_SELECT_IMG	"../assets/editor/spawn_block_select.png"
#define BLANK_MENU			"blank_menu"
#define BLANK_MENU_IMG		"../assets/editor/blank_menu.png"
#define LARGE_MENU			"large_menu"
#define LARGE_MENU_IMG		"../assets/editor/blank_menu_large.png"
#define SMALL_MENU			"small_menu"
#define SMALL_MENU_IMG		"../assets/editor/blank_menu_small.png"
#define DIMENSION			"dimension"
#define DIMENSION_IMG		"../assets/editor/dimension_overlay.png"
#define MISSING				"missing"
#define MISSING_IMG			"../assets/editor/missing.png"

// enumeration representing the overall state of the editor
enum ProgramState {
	STATE_EDITOR,
	STATE_FILE,
	STATE_SETTINGS,
	STATE_MENU
};

// enumeration representing the editing state
enum EditorState {
	EDITOR_EDIT,
	EDITOR_PANNING,
	EDITOR_PICK
};

// enumeration representing the menu state
enum MenuState {
	MENU_SAVEMAP,
	MENU_EDIT_SPAWN,
	MENU_EDIT_PORTAL,
	MENU_MAP_SETTINGS
};

// enumeration representing the edit mode
enum EditMode {
	EDIT_TILE,
	EDIT_COLLISION,
	EDIT_ENTITIES
};

// enumeration representing the currently edited entity
enum EntityEditMode {
	E_EDIT_PLAYER,
	E_EDIT_ENEMY,
	E_EDIT_PORTAL
};

// enumeration representing the types of entities
enum EntityType {
	E_ENEMY
};

// enumeration representing the action taken by file state after choosing
enum FileMode {
	FILE_LOADMAP,
	FILE_LOADTILEMAP,
	FILE_LOADBACKGROUND
};

struct EntityEntry {
	EntityType type;
	int x;
	int y;
};

struct PortalEntry {
	int x;
	int y;
	std::string file;
	int num;
};

// struct representing each file item in the menu
struct FileMenuItem {
	std::string name;
	Math::Rectangle collision;
	Texture * tex;
};