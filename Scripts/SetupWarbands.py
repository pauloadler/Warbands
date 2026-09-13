import unreal


ASSET_ROOT = "/Game/Warbands"
PLAYER_PATH = f"{ASSET_ROOT}/Blueprints/Characters/BP_PlayerCharacter"
GAME_MODE_PATH = f"{ASSET_ROOT}/Blueprints/BP_WarbandsGameMode"


def blueprint_class(asset_path):
    asset_name = asset_path.rsplit("/", 1)[-1]
    return unreal.EditorAssetLibrary.load_blueprint_class(f"{asset_path}.{asset_name}")


def create_blueprint(name, package_path, parent_class):
    asset_path = f"{package_path}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        return unreal.EditorAssetLibrary.load_asset(asset_path)

    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    blueprint = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        name, package_path, unreal.Blueprint, factory
    )
    unreal.EditorAssetLibrary.save_loaded_asset(blueprint)
    return blueprint


for directory in (
    f"{ASSET_ROOT}/Animation",
    f"{ASSET_ROOT}/Blueprints/Characters",
    f"{ASSET_ROOT}/Blueprints/Combat",
    f"{ASSET_ROOT}/Blueprints/Enemies",
    f"{ASSET_ROOT}/Blueprints/Systems",
    f"{ASSET_ROOT}/Data",
    f"{ASSET_ROOT}/Input",
    f"{ASSET_ROOT}/Maps",
    f"{ASSET_ROOT}/UI",
):
    unreal.EditorAssetLibrary.make_directory(directory)

template_player = blueprint_class("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter")
player = create_blueprint("BP_PlayerCharacter", f"{ASSET_ROOT}/Blueprints/Characters", template_player)

template_game_mode = blueprint_class("/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode")
game_mode = create_blueprint("BP_WarbandsGameMode", f"{ASSET_ROOT}/Blueprints", template_game_mode)

player_class = blueprint_class(PLAYER_PATH)
game_mode_class = blueprint_class(GAME_MODE_PATH)
game_mode_cdo = unreal.get_default_object(game_mode_class)
game_mode_cdo.set_editor_property("default_pawn_class", player_class)
unreal.EditorAssetLibrary.save_loaded_asset(game_mode)

unreal.log("Warbands setup finished successfully.")
