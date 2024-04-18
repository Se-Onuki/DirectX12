from typing import Set
import bpy
from bpy.types import Context

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
    print ("レベルエディタが 有効 [O] になりました")

# AddOn無効時のコールバック
def unregister():
    # メニューから項目を削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)

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

# Blenderに登録するクラスのリスト
classes = (
    TOPBAR_MT_my_menu,
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
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