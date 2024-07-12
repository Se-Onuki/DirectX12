from typing import Set
import bpy
from bpy.types import Context
import math
import mathutils
import bpy_extras
import gpu
import gpu_extras.batch
import copy
import json

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

# クラスバーの拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
	# Blenderがクラスを識別する為の固有の文字列
	bl_idname = "TOPBAR_MT_my_menu"
	# メニューのラベルとして表示される文字列
	bl_label = "MyMenu"
	# 著者表示用の文字列
	bl_description = "拡張メニュー by " + bl_info["auther"]

	# サブメニューの描画
	def draw(self, context):

		# トップバーの｢エディターメニュー｣に項目(オペレータ)を追加
		self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')
		self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname, text= MYADDON_OT_stretch_vertex.bl_label)
		self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname, text= MYADDON_OT_create_ico_sphere.bl_label)
		self.layout.operator(MYADDON_OT_export_scene.bl_idname, text= MYADDON_OT_export_scene.bl_label)
	
	# 既存のメニューにサブメニューを追加
	def submenu(self, context):

		# ID指定でサブメニューを追加
		self.layout.menu(TOPBAR_MT_my_menu.bl_idname)

# オペレータ 頂点を伸ばす
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_stretch_vertex"
	bl_label = "頂点を伸ばす"
	bl_description = "頂点座標を引っ張って伸ばします"
	# Redo Undoを可能にするオプション
	bl_options ={"REGISTER", "UNDO"}

	# メニューを実行するときに呼ばれるコールバック関数
	def execute(self, context: Context):
		bpy.data.objects["Cube"].data.vertices[0].co.y += 1.0
		print ("頂点を伸ばしました")

		# オペレータの命令終了を通知
		return {"FINISHED"}

class MYADDON_OT_model_loader(bpy.types.Operator, bpy_extras.io_utils.ImportHelper):
	bl_idname = "myaddon.myaddon_ot_model_loader"
	bl_label = "ModelLoader"
	bl_description = "モデルを読み込み､メッシュに適用します"
	bl_options = {"REGISTER", "UNDO"}

	# メニューを実行するときに呼ばれるコールバック関数
	def execute(self, context: Context):

		# モデルを読み込む
		bpy.ops.import_mesh.stl(filepath=self.filepath)

# オペレータ シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
	bl_idname = "myaddon.myaddon_ot_export_scene"
	bl_label = "シーン出力"
	bl_description = "シーンを出力します"

	# 出力するファイルの拡張子
	filename_ext = ".json"

	def write_and_print(self,file,str):
		print(str)
		file.write(str + "\n")

	# ツリー走査用の再起関数
	def parse_scene_recursive_json(self, data_parent, object, level):
		# シーンのオブジェクト1個分のjsonオブジェクトを生成
		json_object = dict()
		# オブジェクトの種類
		json_object["type"] = object.type
		# オブジェクト名
		json_object["name"] = object.name

		# ローカルTransformから
		# SRTをそれぞれ抽出
		trans, rot, scale = object.matrix_local.decompose()
		# 回転をQuaternionからEulerに変換
		rot = rot.to_euler()
		# 弧度法から度数法に変換
		rot.x = math.degrees(rot.x)
		rot.y = math.degrees(rot.y)
		rot.z = math.degrees(rot.z)
		# Transform情報をディクショナリに登録
		transform = dict()
		transform["translation"] = (trans.x, trans.y, trans.z)
		transform["rotation"] = (rot.x, rot.y, rot.z)
		transform["scale"] = (scale.x, scale.y, scale.z)
		json_object["transform"] = transform

		if ("visiblity" in object):
			json_object["visiblity"] = object["visiblity"]

		# カスタムプロパティ['rigidbody']
		if ("rigidbody" in object):
			rigidbody = dict()
			rigidbody["type"] = object["rigidbody"]
			rigidbody["isKinematic"] = object["isKinematic"]
			rigidbody["gravity"] = object["gravity"]
			json_object["rigidbody"] = rigidbody
		

		# カスタムプロパティ['file_name']
		if ("file_name" in object):
			json_object["directory_name"] = object["directory_name"]
			json_object["file_name"] = object["file_name"]
			
		# カスタムプロパティ['collider']
		if ("collider" in object):
			collider = dict()
			collider["isTrigger"] = object["isTrigger"]
			collider["collision_function"] = object["collision_function"]
			collider["type"] = object["collider"]
			collider["center"] = object["collider_center"].to_list()
			collider["size"] = object["collider_size"].to_list()
			json_object["collider"] = collider

		# 1個分のjsonオブジェクトを親オブジェクトに登録
		data_parent.append(json_object)

		# 子供のリストを走査
		if (len(object.children) > 0):
			# 子ノードリストを作成
			json_object["children"] = list()

			# 子ノードへ進む
			for child in object.children:
				self.parse_scene_recursive_json(json_object["children"], child, level + 1)


	# json形式でファイルに出力
	def export_json(self):

		# 保存する情報をまとめるdict
		json_object_root = dict()

		# ノード名
		json_object_root["name"] = "scene"
		# オブジェクトリストを作成
		json_object_root["objects"] = list()
		# シーンの中のオブジェクトをすべて走査してパックする
		for object in bpy.context.scene.objects:
			# 親オブジェクトがあるなら飛ばす
			if (object.parent):
				continue

			# シーン直下のオブジェクトをルートノード(深さ0)として､再帰関数で走査
			self.parse_scene_recursive_json(json_object_root["objects"], object, 0)

		# オブジェクトをJson文字列にエンコード
		json_text = json.dumps(json_object_root, ensure_ascii=False, cls = json.JSONEncoder, indent = 4)
		# コンソールに表示する
		print(json_text)

		# ファイルをテキスト形式で書き出し用にオープン
		# スコープを抜けると自動的にクローズされる
		with open(self.filepath, "wt", encoding="utf-8") as file:
			# ファイルに文字列を書き込む
			file.write(json_text)

	def parse_scene_recursive(self, file, object, level):
		"""シーン解析用再帰関数"""

		# インデントを作成する
		indent = ''
		for i in range(level):
			indent += '\t'


		# オブジェクト名の書き込み
		self.write_and_print (file, indent + object.type)
		# ローカルのトランスフォームから､平行移動､回転､拡大縮小を取得
		trans, rot, scale = object.matrix_local.decompose()
		# 回転をオイラーに変換
		rot = rot.to_euler()
		# ラジアンから度数法に変更
		rot.x = math.degrees(rot.x)
		rot.y = math.degrees(rot.y)
		rot.z = math.degrees(rot.z)
		# トランスフォームを表示
		self.write_and_print (file, indent + "T %f %f %f" % (trans.x, trans.y, trans.z))
		self.write_and_print (file, indent + "R %f %f %f" % (rot.x, rot.y, rot.z))
		self.write_and_print (file, indent + "S %f %f %f" % (scale.x, scale.y, scale.z))

		# カスタムプロパティ['file_name']
		if "file_name" in object:
			self.write_and_print (file, indent + "N %s" % object["directory_name"])
			self.write_and_print (file, indent + "N %s" % object["file_name"])
		# カスタムプロパティ['collider']
		if "collider" in object:
			self.write_and_print(file, indent + "C %s" % object["collider"])
			temp_str = indent + "CC %f %f %f" % (object["collider_center"][0], object["collider_center"][1], object["collider_center"][2])
			self.write_and_print(file, temp_str)
			temp_str = indent + "CS %f %f %f" % (object["collider_size"][0], object["collider_size"][1], object["collider_size"][2])
			self.write_and_print(file, temp_str)

		self.write_and_print (file,indent + "END")
		self.write_and_print (file,'')

		# 子ノードに進む
		for child in object.children:
			self.parse_scene_recursive(file, child, level + 1)


	def export(self):
		"""ファイルに出力"""

		print ("シーン情報出力開始... %r" % self.filepath)
		# ファイルをテキスト形式で書き出し用にオープン
		# スコープを抜けると自動的にクローズされる
		with open(self.filepath, "wt") as file:
			# ファイルに文字列を書き込む

			# シーンの中のオブジェクトをすべて走査する｡
			for object in bpy.context.scene.objects:
				
				# 親オブジェクトがあるなら飛ばす
				if object.parent:
					continue

				# シーン直下のオブジェクトをルートノード(深さ0)として､再帰関数で走査
				self.parse_scene_recursive (file, object, 0)

	# メニューを実行するときに呼ばれるコールバック関数
	def execute(self, context):
		# シーンを出力
		print ("シーン情報をExportします")

		self.export_json()

		print("シーン情報をExportしました")
		self.report({'INFO'}, "シーン情報をExportしました")

		# オペレータの命令終了を通知
		return {"FINISHED"}

# オペレータ ICO球の生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_create_ico_sphere"
	bl_label = "ICO球の生成"
	bl_description = "ICO球を生成します"
	bl_options = {"REGISTER", "UNDO"}

	# メニューを実行するときに呼ばれるコールバック関数
	def execute(self, context):
		# ico球を生成 参考url: https://docs.blender.org/api/current/bpy.ops.mesh.html#bpy.ops.mesh.primitive_ico_sphere_add
		bpy.ops.mesh.primitive_ico_sphere_add()
		print ("ICO球を生成しました")

		# オペレータの命令終了を通知
		return {"FINISHED"}
	
# コライダ描画
class DrawCollider:
	# 描画ハンドル
	handle = None
	# 3Dビューに登録する描画関数
	def draw_collider():
		# 頂点データ
		vertices = {"pos":[]}
		# Indexデータ
		indices = []
		# 各頂点の、オブジェクト中心からのオフセット
		offsets = [
			[-0.5, -0.5, -0.5], # 左下前
			[+0.5, -0.5, -0.5], # 右下前
			[-0.5, +0.5, -0.5], # 左上前
			[+0.5, +0.5, -0.5], # 右上前
			
			[-0.5, -0.5, +0.5], # 左下奥
			[+0.5, -0.5, +0.5], # 右下奥
			[-0.5, +0.5, +0.5], # 左上奥
			[+0.5, +0.5, +0.5], # 右上奥
		]
		# 立方体のサイズ
		size = [2, 2, 2]
		
		# 現在シーンのオブジェクトリストを走査
		for object in bpy.context.scene.objects:
			
			# もしコライダプロパティがなければ描画をスキップ
			if not "collider" in object:
				continue

			# オブジェクトの座標データ
			center = mathutils.Vector((0,0,0))
			size = mathutils.Vector((2,2,2))

			# プロパティからデータを取得
			center[0] = object["collider_center"][0]
			center[1] = object["collider_center"][1]
			center[2] = object["collider_center"][2]
			size[0] = object["collider_size"][0]
			size[1] = object["collider_size"][1]
			size[2] = object["collider_size"][2]

			# 追加前の頂点数
			start = len(vertices['pos'])
			
			# Boxの8頂点分回す
			for offset in offsets:
				# オブジェクトの中心座標をコピー
				pos = copy.copy(center)
				# 中心点を基準に各頂点ごとにずらす
				pos[0] += offset[0] * size[0]
				pos[1] += offset[1] * size[1]
				pos[2] += offset[2] * size[2]
				# グローバル座標に変換
				pos = object.matrix_world @ pos
				# 頂点リストに座標を追加
				vertices['pos'].append(pos)

				# 前面を構成する辺の頂点Index
				indices.append([start + 0, start + 1])
				indices.append([start + 2, start + 3])
				indices.append([start + 0, start + 2])
				indices.append([start + 1, start + 3])
				# 奥面を構成する辺の頂点Index
				indices.append([start + 4, start + 5])
				indices.append([start + 6, start + 7])
				indices.append([start + 4, start + 6])
				indices.append([start + 5, start + 7])
				# 前面と奥面を繋ぐ辺の頂点Index
				indices.append([start + 0, start + 4])
				indices.append([start + 1, start + 5])
				indices.append([start + 2, start + 6])
				indices.append([start + 3, start + 7])

		# ビルトインのシェーダを取得
		shader = gpu.shader.from_builtin("UNIFORM_COLOR")
		# バッチを作成(引数 : シェーダ、トポロジー、頂点データ、Indexデータ)
		batch = gpu_extras.batch.batch_for_shader(shader, "LINES", vertices, indices = indices)

		# シェーダのパラメータ設定
		color = [0.5, 1.0, 1.0, 1.0]
		shader.bind()
		shader.uniform_float("color", color)
		# 描画
		batch.draw(shader)

# オペレータ カスタムプロパティ[ 'collider' ]追加
class MYADDON_OT_add_collider(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_collider"
	bl_label = "コライダ 追加"
	bl_description = "[ 'collider' ]カスタムプロパティを追加します"
	bl_options = {"REGISTER", "UNDO"}

	def execute(self, context):
		#[ 'collider' ]カスタムプロパティを追加
		context.object["collider"] = "BOX"
		context.object["isTrigger"] = False
		context.object["collider_center"] = mathutils.Vector((0,0,0))
		context.object["collider_size"] = mathutils.Vector((2,2,2))
		context.object["collision_function"] = ""

		return {"FINISHED"}
	

# オペレータ カスタムプロパティ[ 'visiblity' ]追加
class MYADDON_OT_add_visibility(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_visibility"
	bl_label = "可視性フラグの追加"
	bl_description = "[ 'visiblity' ]カスタムプロパティを追加します"
	bl_options = {"REGISTER", "UNDO"}

	def execute(self, context):
		#[ 'visiblity' ]カスタムプロパティを追加
		context.object["visiblity"] = True
		return {"FINISHED"}
	
class MYADDON_OT_import_mesh(bpy.types.Operator, bpy_extras.io_utils.ImportHelper):
	bl_idname = "myaddon.myaddon_ot_import_mesh"
	bl_label = "Import Mesh"
	bl_options = {'REGISTER', 'UNDO'}

	filename_ext = ".gltf"  # You can change this to the file type you want to support

	filter_glob: bpy.props.StringProperty(
		 default="*.gltf;*.glb",
		 options={'HIDDEN'},
		 maxlen=255,
	)

	def execute(self, context):
		filepath = self.filepath
		self.import_mesh(context, filepath)
		return {'FINISHED'}

	def import_mesh(self, context, filepath):
		
		# Deselect all objects
		bpy.ops.object.select_all(action='DESELECT')

		# Select the active object (the one to be replaced)
		obj = context.active_object
		# if obj is None or obj.type != 'MESH':
		# 	self.report({'ERROR'}, "Active object is not a mesh")
		# 	return

		# 文字列の末尾から最初の'\'までの文字をfileNameに代入
		fileName = filepath.split('\\')[-1]

		# 'resources\'までの文字をdirectoryに代入
		directory_index = filepath.rfind('resources\\')
		if directory_index != -1:
			directory = filepath.split('resources\\')[-1].split(fileName)[0].replace('\\', '/')
		else:
			directory = ''
		
		obj["directory_name"] = directory
		obj["file_name"] = fileName

		# # データを保存する
		# obj_location = obj.location
		# obj_rotation = obj.rotation_euler
		# obj_scale = obj.scale
		obj_custom_props = obj.items()

		# gltfメッシュを読み込む
		bpy.ops.import_scene.gltf(filepath=filepath)

		# # 読み込んだメッシュにデータを渡す
		new_obj = context.selected_objects[0]
		# 	new_obj.location += obj_location
		# 	#new_obj.rotation_euler = obj_rotation
		# 	#new_obj.scale *= obj_scale
		for key, value in obj_custom_props:
			new_obj[key] = value

		# # 古い方のデータを破棄
		bpy.data.objects.remove(obj, do_unlink=True)

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



# オペレータ カスタムプロパティ['file_name']を追加
class MYADDON_OT_add_modeldata(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_modeldata"
	bl_label = "ModelData 追加"
	bl_description = "カスタムプロパティ['file_name']を追加します"
	bl_options = {"REGISTER", "UNDO"}

	def execute(self, context):
		
		# カスタムプロパティ['directory_name']を追加
		context.object["directory_name"] = ""
		# カスタムプロパティ['file_name']を追加
		context.object["file_name"] = ""

		return {"FINISHED"}

# オペレータ カスタムプロパティ['rigidbody']を追加
class MYADDON_OT_add_rigidbody(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_rigidbody"
	bl_label = "rigidbody 追加"
	bl_description = "カスタムプロパティ['rigidbody']を追加します"
	bl_options = {"REGISTER", "UNDO"}

	def execute(self, context):
		
		# カスタムプロパティ['rigidbody']を追加
		context.object["rigidbody"] = True
		context.object["isKinematic"] = True
		context.object["gravity"] = -9.8

		return {"FINISHED"}




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
)

# メニュー項目描画
def draw_menu_manual(self, context):
	# self : 呼び出し元のクラスインスタンス｡ C++ でいうthisポインタ
	# context : カーソルを合わせた時のポップアップのカスタマイズなどに使用される

	# トップバーの｢エディターメニュー｣に項目(オペレータ)を追加
	self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')

# テスト実行用のコード
#if __name__ == "__main__":  # アドオンのときは自動的に呼ばれる
#   register()