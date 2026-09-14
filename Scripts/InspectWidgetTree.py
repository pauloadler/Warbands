import unreal

widget = unreal.EditorAssetLibrary.load_asset("/Game/Warbands/UI/WBP_CharacterCreation")
try:
    tree = widget.get_editor_property("widget_tree")
    unreal.log("Widget tree object: " + str(tree))
    unreal.log("Widget tree methods: " + str(dir(tree)))
except Exception as error:
    unreal.log("Widget tree error: " + str(error))
