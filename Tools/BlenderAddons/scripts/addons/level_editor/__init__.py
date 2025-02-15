from typing import Set
import bpy

# Blenderに登録するアドオン情報
bl_info = {
	"name": "レベルエディタ",
	"auther":"ONUKI Seiya",
	"version": (0, 0, 1),
	"blender": (4, 1, 0),
	"location": "",
	"description": "レベルエディタ",
	"warning": "",
	#"support": 'TESTING', 最新バージョンは消えている
	"wiki_url": "",
	"tracker_url": "",
	"category": "Object",
}

# オペレータ 頂点を伸ばす
from .blpy_test import *

from .model_io import *

from .colider import *

from .menu import *

from .player_spawn import *

from .spawn import *

# AddOn有効時のコールバック
def register():
	# Blenderにクラスを登録する
	for cls in classes:
		bpy.utils.register_class(cls)

	# メニューに項目を追加
	bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
	# 3Dビューに描画関数を追加
	DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider, (), "WINDOW", "POST_VIEW")
	print ("レベルエディタが 有効 [O] になりました")

# AddOn無効時のコールバック
def unregister():
	# メニューから項目を削除
	bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
	# 3Dビューから描画関数を削除
	bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle, "WINDOW")

	# Blenderからクラスを削除する
	for cls in classes:
		bpy.utils.unregister_class(cls)
	print ("レベルエディタが 無効 [X] になりました")


# Blenderに登録するクラスのリスト
classes = (
	TOPBAR_MT_my_menu,
	MYADDON_OT_stretch_vertex,
	MYADDON_OT_create_ico_sphere,
	MYADDON_OT_export_scene,
	MYADDON_OT_add_modeldata,
	MYADDON_OT_add_collider,
	MYADDON_OT_add_rigidbody,
	MYADDON_OT_add_visibility,
	MYADDON_OT_import_mesh,
	OBJECT_PT_component,
	MYADDON_OT_add_model,
	MYADDON_OT_spawn_point_import,
	MYADDON_OT_spawn_point_generate,
	MYADDON_OT_player_spawn_point,
	MYADDON_OT_enemy_spawn_point,
)

# メニュー項目描画
def draw_menu_manual(self, context):
	# self : 呼び出し元のクラスインスタンス｡ C++ でいうthisポインタ
	# context : カーソルを合わせた時のポップアップのカスタマイズなどに使用される

	# トップバーの｢エディターメニュー｣に項目(オペレータ)を追加
	self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')
