from typing import Set
import bpy
from bpy.types import Context
import math
import mathutils
import bpy_extras
import gpu
import gpu_extras.batch
import copy

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

# オペレータ シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
	bl_idname = "myaddon.myaddon_ot_export_scene"
	bl_label = "シーン出力"
	bl_description = "シーンを出力します"

	# 出力するファイルの拡張子
	filename_ext = ".scene"

	def write_and_print(self,file,str):
		print(str)
		file.write(str + "\n")

	def parse_scene_recursive(self, file, object, level):
		"""シーン解析用再帰関数"""

		# インデントを作成する
		indent = ''
		for i in range(level):
			indent += '\t'


		# オブジェクト名の書き込み
		self.write_and_print (file, indent + object.type + " - " + object.name)
		# ローカルのトランスフォームから､平行移動､回転､拡大縮小を取得
		trans, rot, scale = object.matrix_local.decompose()
		# 回転をオイラーに変換
		rot = rot.to_euler()
		# ラジアンから度数法に変更
		rot.x = math.degrees(rot.x)
		rot.y = math.degrees(rot.y)
		rot.z = math.degrees(rot.z)
		# トランスフォームを表示
		self.write_and_print (file, indent + "Trans(%f, %f, %f)" % (trans.x, trans.y, trans.z))
		self.write_and_print (file, indent + "Rot(%f, %f, %f)" % (rot.x, rot.y, rot.z))
		self.write_and_print (file, indent + "Scale(%f, %f, %f)" % (scale.x, scale.y, scale.z))
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

		self.export()

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
			# 追加前の頂点数
			start = len(vertices['pos'])
			
			# Boxの8頂点分回す
			for offset in offsets:
				# オブジェクトの中心座標をコピー
				pos = copy.copy(object.location)
				# 中心点を基準に各頂点ごとにずらす
				pos[0] += offset[0] * size[0]
				pos[1] += offset[1] * size[1]
				pos[2] += offset[2] * size[2]
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
		batch = gpu_extras.batch.batch_for_shader(shader,"LINES", vertices, indices = indices)

		# シェーダのパラメータ設定
		color = [0.5, 1.0, 1.0, 1.0]
		shader.bind()
		shader.uniform_float("color", color)
		# 描画
		batch.draw(shader)

# Blenderに登録するクラスのリスト
classes = (
	TOPBAR_MT_my_menu,
	MYADDON_OT_stretch_vertex,
	MYADDON_OT_create_ico_sphere,
	MYADDON_OT_export_scene,
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