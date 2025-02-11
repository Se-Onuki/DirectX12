import bpy
from bpy.types import Context
import math
import mathutils
import gpu
import gpu_extras.batch
import copy

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
	