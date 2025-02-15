import bpy
import os

class SpawnNames():

	# インデックス
	PROTOTYPE = 0
	INSTANCE = 1
	FILENAME = 2

	names = {}

	# names["キー"] = ["プロトタイプ名", "インスタンス名", "ファイル名"]
	names["Enemy"]  = ("prototype_enemy", "enemy_spawn_point", "enemy_model/box.obj")		# 敵
	names["Player"] = ("prototype_player", "player_spawn_point", "player_model/Orb.obj")	# プレイヤー

# オペレータ 出現ポイントのシンボルを読み込む
class MYADDON_OT_spawn_point_import(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_spawn_point_import"
	bl_label = "出現ポイントのシンボルを読み込む"
	bl_description = "出現ポイントのシンボルを読み込みます"


	def load_model(self, type):
		print("出現ポイントを読み込みます")

		# 重複ロードの対策
		# 既にオブジェクトが存在しているのなら､取得する
		spawn_object = bpy.data.objects.get(SpawnNames.names[type][SpawnNames.PROTOTYPE])
		if spawn_object is not None:
			return {'CANCELLED'}

		# スクリプトが配置されているディレクトリの名前を取得
		addon_dir = os.path.dirname(__file__)
		# ディレクトリからのモデルファイルの相対パスを記述
		relative_path = SpawnNames.names[type][SpawnNames.FILENAME]
		# モデルファイルのフルパスを取得
		full_path = os.path.join(addon_dir, relative_path)
		# objメッシュを読み込む
		bpy.ops.wm.obj_import('EXEC_DEFAULT', filepath=full_path, display_type = 'THUMBNAIL', forward_axis = 'Z', up_axis = 'Y')
		# 回転の状態を適用
		bpy.ops.object.transform_apply(location=False, rotation=True, scale=False)

		# アクティブなオブジェクトを取得する
		object = bpy.context.active_object
		# オブジェクトの名前を変更	
		object.name = SpawnNames.names[type][SpawnNames.PROTOTYPE]

		# オブジェクトの種類を指定		
		object["type"] = SpawnNames.names[type][SpawnNames.INSTANCE]

		# メモリには残すが､シーンには表示しない
		bpy.context.collection.objects.unlink(object)

		return {'FINISHED'}
	
	def execute(self, context):
		# Enemyオブジェクトを読み込む
		self.load_model("Enemy")
		# Playerオブジェクトを読み込む
		self.load_model("Player")

		return {'FINISHED'}
	
# オペレータ 出現ポイントの生成
class MYADDON_OT_spawn_point_generate(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_spawn_point_generate"
	bl_label = "出現ポイントの生成"
	bl_description = "出現ポイントの生成を行います"
	
	# プロパティ(引数として渡される)
	type : bpy.props.StringProperty(name = "Type", default = "Player")

	def execute(self, context):
		print("出現ポイントを生成します")

		# コピー元オブジェクトを検索
		spawn_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE])

		# まだ読み込んでいない場合
		if spawn_object is None:
			# 読み込みオペレータを実行する
			bpy.ops.myaddon.myaddon_ot_spawn_point_import('EXEC_DEFAULT')
			# コピー元オブジェクトを検索
			spawn_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE])

		# Blenderでの選択を解除する
		bpy.ops.object.select_all(action='DESELECT')

		# 複製元の非表示オブジェクトを複製する
		copy_object = spawn_object.copy()
		# 複製したオブジェクトを現在のシーンにリンク(出現させる)
		bpy.context.collection.objects.link(copy_object)
		# オブジェクト名を変更
		copy_object.name = SpawnNames.names[self.type][SpawnNames.INSTANCE]

		return {'FINISHED'}
	