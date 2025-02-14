from typing import Set
import bpy
from bpy.types import Context
import math
import bpy_extras
import json

from .sol_utils import *

# オペレータ モデルを読み込んで追加する
class MYADDON_OT_add_model(bpy.types.Operator, bpy_extras.io_utils.ImportHelper):
	bl_idname = "myaddon.myaddon_ot_add_model"
	bl_label = "モデルObject追加"
	bl_description = "モデルファイルからObjectとして追加します"
	# Redo Undoを可能にするオプション
	bl_options ={"REGISTER", "UNDO"}

	filter_glob: bpy.props.StringProperty(
		 default="*.gltf;*.glb;*.obj",
		 options={'HIDDEN'},
		 maxlen=255,
	)

	# メニューを実行するときに呼ばれるコールバック関数
	def execute(self, context: Context):
		filepath = self.filepath
		self.import_mesh(context, filepath)
		return {'FINISHED'}

	# モデルを読み込む
	def import_mesh(self, context, filepath):
		
		# 文字列の末尾から最初の'\'までの文字をfileNameに代入
		fileName = filepath.split('\\')[-1]

		# 'resources\'までの文字をdirectoryに代入
		directory_index = filepath.rfind('resources\\')
		if directory_index != -1:
			directory = filepath.split('resources\\')[-1].split(fileName)[0].replace('\\', '/')
		else:
			directory = ''
		
		# もし､読み込むファイルの末尾が".obj"ならば
		if(filepath.split('.')[-1] == 'obj'):
			bpy.ops.wm.obj_import(filepath=filepath)
		else:
		# gltfメッシュを読み込む
			bpy.ops.import_scene.gltf(filepath=filepath, bone_heuristic = 'TEMPERANCE')

		# # 読み込んだメッシュにデータを渡す
		new_obj = context.selected_objects[0]

		# 回転の状態を適用
		bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)
		
		new_obj["directory_name"] = directory
		new_obj["file_name"] = fileName


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

	filter_glob: bpy.props.StringProperty(
		 default="*.json;*.jsonc",
		 options={'HIDDEN'},
		 maxlen=255,
	)

	def write_and_print(self,file,str):
		print(str)
		file.write(str + "\n")

	# ツリー走査用の再起関数
	def parse_scene_recursive_json(self, data_parent, object, level):
		# シーンのオブジェクト1個分のjsonオブジェクトを生成
		json_object = dict()
		# オブジェクトの種類
		if 'type' in object:
			json_object["type"] = object["type"]
		else:
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

# メッシュ読み込みオペレータ
class MYADDON_OT_import_mesh(bpy.types.Operator, bpy_extras.io_utils.ImportHelper):
	bl_idname = "myaddon.myaddon_ot_import_mesh"
	bl_label = "Import Mesh"
	bl_options = {'REGISTER', 'UNDO'}

	filename_ext = ".gltf"  # You can change this to the file type you want to support

	filter_glob: bpy.props.StringProperty(
		 default="*.gltf;*.glb;*.obj",
		 options={'HIDDEN'},
		 maxlen=255,
	)

	def execute(self, context):
		filepath = self.filepath
		self.import_mesh(context, filepath)
		return {'FINISHED'}

	def import_mesh(self, context, filepath):
		
		# すべてのオブジェクトの選択を解除する
		bpy.ops.object.select_all(action='DESELECT')

		# アクティブなオブジェクト(置き換える対象)を選択する
		obj = context.active_object

		# 文字列の末尾から最初の'\'までの文字をfileNameに代入
		fileName = filepath.split('\\')[-1]

		# 'resources\'までの文字をdirectoryに代入
		directory_index = filepath.rfind('resources\\')
		if directory_index != -1:
			directory = filepath.split('resources\\')[-1].split(fileName)[0].replace('\\', '/')
		else:
			directory = ""
		
		obj["directory_name"] = directory
		obj["file_name"] = fileName

		# # データを保存する
		# obj_location = obj.location
		# obj_rotation = obj.rotation_euler
		# obj_scale = obj.scale
		obj_custom_props = obj.items()
		
		# もし､読み込むファイルの末尾が".obj"ならば
		if(filepath.split('.')[-1] == 'obj'):
			bpy.ops.wm.obj_import(filepath=filepath)
		else:
		# gltfメッシュを読み込む
			bpy.ops.import_scene.gltf(filepath=filepath, bone_heuristic = 'TEMPERANCE')

		# # 読み込んだメッシュにデータを渡す
		new_obj = context.selected_objects[0]
		# 	new_obj.location += obj_location
		# 	#new_obj.rotation_euler = obj_rotation
		# 	#new_obj.scale *= obj_scale
		for key, value in obj_custom_props:
			new_obj[key] = value

		# 回転の状態を適用
		bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)
		
		# # 古い方のデータを破棄
		delete_hierarchy(obj)