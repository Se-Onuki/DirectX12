import bpy
from .model_io import *
from .colider import *
from .component import *
from .player_spawn import *

# クラスバーの拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
	# Blenderがクラスを識別する為の固有の文字列
	bl_idname = "TOPBAR_MT_my_menu"
	# メニューのラベルとして表示される文字列
	bl_label = "MyMenu"
	# 著者表示用の文字列
	bl_description = "拡張メニュー by " + "ONUKI Seiya"

	# サブメニューの描画
	def draw(self, context):

		# トップバーの｢エディターメニュー｣に項目(オペレータ)を追加
		self.layout.operator(MYADDON_OT_add_model.bl_idname, text= MYADDON_OT_add_model.bl_label)
		self.layout.operator(MYADDON_OT_export_scene.bl_idname, text= MYADDON_OT_export_scene.bl_label)
		self.layout.operator(MYADDON_OT_player_spawn_point.bl_idname, text= MYADDON_OT_player_spawn_point.bl_label)
		

	# 既存のメニューにサブメニューを追加
	def submenu(self, context):

		# ID指定でサブメニューを追加
		
		self.layout.menu(TOPBAR_MT_my_menu.bl_idname)


# パネル ファイル名
class OBJECT_PT_component(bpy.types.Panel):
	"""オブジェクトのファイルネームパネル"""
	bl_idname = "OBJECT_PT_component"
	bl_label = "Component Editor"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "object"

	def draw(self, context):
		layout = self.layout
		if "visiblity" in context.object:
			layout.prop(context.object, '["visiblity"]', text ="Visiblity")
		else:
			layout.operator(MYADDON_OT_add_visibility.bl_idname)
		# パネルに項目を追加
		if "collider" in context.object:
			layout.label(text = "Collider",icon = "DOT")
			# すでにプロパティがあれば､プロパティを表示
			layout.prop(context.object, '["isTrigger"]', text ="IsTrigger")
			layout.prop(context.object, '["collision_function"]', text ="CollisionFunction")
			layout.prop(context.object, '["collider"]', text ="Type")
			layout.prop(context.object, '["collider_center"]', text ="Center")
			layout.prop(context.object, '["collider_size"]', text ="Size")
			layout.separator()
		else:
			layout.operator(MYADDON_OT_add_collider.bl_idname)
		# パネルに項目を追加
		if "file_name" in context.object:
			layout.label(text = "ModelName",icon = "DOT")
			layout.prop(context.object, '["directory_name"]', text = "DirectoryName")
			layout.prop(context.object, '["file_name"]', text = "FileName")
			layout.operator(MYADDON_OT_import_mesh.bl_idname)
			layout.separator()
		else:
			layout.operator(MYADDON_OT_add_modeldata.bl_idname)
		# パネルに項目を追加
		if "rigidbody" in context.object:
			layout.label(text = "Rigidbody",icon = "DOT")
			layout.prop(context.object, '["isKinematic"]', text = "IsKinematic")
			layout.prop(context.object, '["gravity"]', text = "Gravity")
			layout.separator()
		else:
			layout.operator(MYADDON_OT_add_rigidbody.bl_idname)
