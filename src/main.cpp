#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/web.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

using namespace geode::prelude;

class $modify(MationBuildEditorLayer, LevelEditorLayer) {
    struct Fields {
        async::TaskHolder<web::WebResponse> m_keyCheck;
        async::TaskHolder<file::PickResult> m_filePick;
    };

    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) return false;
        if (noUI) return true;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto autoMenu = CCMenu::create();
        if (autoMenu) {
            autoMenu->setID("solance.mationbuild/auto-menu");
            autoMenu->setPosition({winSize.width - 72.f, winSize.height - 42.f});
            auto autoSprite = ButtonSprite::create(
                "Auto", 58, 0, 0.65f, true, "goldFont.fnt", "GJ_button_01.png", 0.f
            );
            if (autoSprite) {
                auto autoButton = CCMenuItemSpriteExtra::create(
                    autoSprite, this, menu_selector(MationBuildEditorLayer::onMationBuild)
                );
                if (autoButton) {
                    autoButton->setID("solance.mationbuild/auto-decorate");
                    autoMenu->addChild(autoButton);
                    this->addChild(autoMenu, 1000);
                }
            }
        }

        auto importMenu = CCMenu::create();
        if (importMenu) {
            importMenu->setID("solance.mationbuild/import-menu");
            importMenu->setPosition({winSize.width - 152.f, winSize.height - 42.f});
            auto importSprite = ButtonSprite::create(
                "Import", 64, 0, 0.65f, true, "goldFont.fnt", "GJ_button_01.png", 0.f
            );
            if (importSprite) {
                auto importButton = CCMenuItemSpriteExtra::create(
                    importSprite, this, menu_selector(MationBuildEditorLayer::onImportTemplate)
                );
                if (importButton) {
                    importButton->setID("solance.mationbuild/import-template");
                    importMenu->addChild(importButton);
                    this->addChild(importMenu, 1000);
                }
            }
        }
        return true;
    }

    void onMationBuild(CCObject*) {
        this->verifyAccess([this] { this->applyAutoDecoration(); });
    }

    void onImportTemplate(CCObject*) {
        this->verifyAccess([this] { this->chooseTemplateFile(); });
    }

    void verifyAccess(std::function<void()> onSuccess) {
        auto key = Mod::get()->getSettingValue<std::string>("access-key");
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
        if (key.size() != 23 || key.rfind("MB-", 0) != 0 ||
            !std::all_of(key.begin(), key.end(), [](unsigned char c) {
                return std::isalnum(c) || c == '-';
            })) {
            FLAlertLayer::create(
                "MationBuild",
                "Add your access key in Geode's MationBuild mod settings, then try again.",
                "Close"
            )->show();
            return;
        }

        auto req = web::WebRequest();
        req.header("Content-Type", "application/json");
        req.header("apikey", "sb_publishable_w0puiuo49MRiVpf4sQWj9Q_67R-YnCO");
        // The key is restricted to alphanumeric characters and hyphens before JSON insertion.
        req.bodyString(fmt::format(R"({{"action":"verify","key":"{}"}})", key));

        m_fields->m_keyCheck.spawn(
            "Verify MationBuild access key",
            req.post("https://gfgmknlxzdfnjdpxetdz.supabase.co/functions/v1/mationbuild-api"),
            [this, onSuccess = std::move(onSuccess)](web::WebResponse response) {
                auto body = response.string().unwrapOr("");
                if (!response.ok() || body.find("\"valid\":true") == std::string::npos) {
                    FLAlertLayer::create(
                        "MationBuild",
                        "Your access key could not be verified. Check that it is active and that you are online.",
                        "Close"
                    )->show();
                    return;
                }
                onSuccess();
            }
        );
    }

    void applyAutoDecoration() {
        auto ui = m_editorUI;
        if (!ui) {
            FLAlertLayer::create("MationBuild", "The editor UI is not ready yet.", "Close")->show();
            return;
        }

        auto selected = ui->m_selectedObjects;
        if ((!selected || selected->count() == 0) && !ui->m_selectedObject) {
            FLAlertLayer::create(
                "MationBuild",
                "Select one or more objects first. MationBuild adds small decorative accents only around your selection.",
                "Close"
            )->show();
            return;
        }

        auto targets = selected && selected->count() > 0
            ? selected
            : CCArray::createWithObject(ui->m_selectedObject);
        if (targets->count() > 100) {
            FLAlertLayer::create(
                "MationBuild",
                "Please select 100 objects or fewer for this pass to prevent accidental level clutter.",
                "Close"
            )->show();
            return;
        }

        struct Accent {
            int id;
            CCPoint offset;
            float scale;
        };
        const Accent tech[] = {
            {1764, {-10.f, 10.f}, 0.35f},
            {1765, { 10.f,-10.f}, 0.35f},
            {1767, {-10.f,-10.f}, 0.35f},
            {1768, { 10.f, 10.f}, 0.35f}
        };
        const Accent minimal[] = {
            {1764, {-8.f, 8.f}, 0.28f},
            {1764, { 8.f,-8.f}, 0.28f},
            {1764, {-8.f,-8.f}, 0.28f},
            {1764, { 8.f, 8.f}, 0.28f}
        };
        const Accent geometric[] = {
            {1766, {-10.f, 10.f}, 0.35f},
            {1765, { 10.f,-10.f}, 0.35f},
            {1764, {-10.f,-10.f}, 0.35f},
            {1767, { 10.f, 10.f}, 0.35f}
        };
        auto style = Mod::get()->getSettingValue<std::string>("style");
        const Accent* accents = tech;
        if (style == "Minimal") accents = minimal;
        else if (style == "Geometric") accents = geometric;

        gd::string payload;
        int requested = 0;
        for (unsigned int i = 0; i < targets->count(); i++) {
            auto target = typeinfo_cast<GameObject*>(targets->objectAtIndex(i));
            if (!target) continue;
            for (int j = 0; j < 4; j++) {
                auto const& accent = accents[j];
                auto decoration = GameObject::createWithKey(accent.id);
                if (!decoration) continue;
                decoration->setPosition(target->getPosition() + accent.offset);
                decoration->setScale(accent.scale);
                payload += decoration->getSaveString(this);
                payload += ";";
                requested++;
            }
        }

        this->pasteGeneratedObjects(payload, requested);
    }

    void chooseTemplateFile() {
        file::FilePickOptions options;
        options.filters.push_back({"MationBuild JSON template", {"*.json"}});
        m_fields->m_filePick.spawn(
            "Choose a MationBuild template",
            file::pick(file::PickMode::OpenFile, options),
            [this](file::PickResult result) {
                if (!result) {
                    FLAlertLayer::create("MationBuild", "Could not open the file picker.", "Close")->show();
                    return;
                }
                auto chosen = result.unwrap();
                if (!chosen) return;

                std::error_code ec;
                auto size = std::filesystem::file_size(*chosen, ec);
                if (ec || size == 0 || size > 500'000) {
                    FLAlertLayer::create(
                        "MationBuild",
                        "Template files must be between 1 byte and 500 KB.",
                        "Close"
                    )->show();
                    return;
                }

                auto contents = file::readString(*chosen);
                if (!contents) {
                    FLAlertLayer::create("MationBuild", "Could not read that template file.", "Close")->show();
                    return;
                }
                this->applyTemplateJson(contents.unwrap());
            }
        );
    }

    void applyTemplateJson(std::string const& source) {
        auto parsed = matjson::parse(source);
        if (!parsed) {
            FLAlertLayer::create("MationBuild", "That file is not valid JSON.", "Close")->show();
            return;
        }
        auto root = parsed.unwrap();
        if (!root.isObject() ||
            root["format"].asString().unwrapOr("") != "mationbuild-json" ||
            root["schema_version"].asInt().unwrapOr(0) != 1 ||
            root["game_version"].asString().unwrapOr("") != "2.2081" ||
            !root["objects"].isArray()) {
            FLAlertLayer::create(
                "MationBuild",
                "Unsupported template. Expected mationbuild-json schema version 1 for Geometry Dash 2.2081.",
                "Close"
            )->show();
            return;
        }

        auto objectResult = root["objects"].asArray();
        if (!objectResult) {
            FLAlertLayer::create("MationBuild", "Template objects must be a JSON array.", "Close")->show();
            return;
        }
        auto const& objectValues = objectResult.unwrap();
        if (objectValues.empty() || objectValues.size() > 100) {
            FLAlertLayer::create("MationBuild", "Templates must contain between 1 and 100 objects.", "Close")->show();
            return;
        }

        struct TemplateObject {
            int id;
            float x;
            float y;
            float scale;
            float rotation;
        };
        std::vector<TemplateObject> objects;
        objects.reserve(objectValues.size());
        for (auto const& value : objectValues) {
            if (!value.isObject()) {
                FLAlertLayer::create("MationBuild", "Every template object must be an object.", "Close")->show();
                return;
            }
            auto id = static_cast<int>(value["id"].asInt().unwrapOr(-1));
            auto x = value["x"].asDouble().unwrapOr(0.0);
            auto y = value["y"].asDouble().unwrapOr(0.0);
            auto scale = value["scale"].asDouble().unwrapOr(0.35);
            auto rotation = value["rotation"].asDouble().unwrapOr(0.0);

            // The portable format deliberately whitelists tiny non-trigger accents only.
            if (id < 1764 || id > 1768 || !std::isfinite(x) || !std::isfinite(y) ||
                !std::isfinite(scale) || !std::isfinite(rotation) ||
                std::abs(x) > 3000.0 || std::abs(y) > 3000.0 ||
                scale < 0.1 || scale > 2.0 || std::abs(rotation) > 3600.0) {
                FLAlertLayer::create(
                    "MationBuild",
                    "Template contains an unsupported object ID or out-of-range transform.",
                    "Close"
                )->show();
                return;
            }
            objects.push_back({id, static_cast<float>(x), static_cast<float>(y),
                static_cast<float>(scale), static_cast<float>(rotation)});
        }

        auto ui = m_editorUI;
        if (!ui) {
            FLAlertLayer::create("MationBuild", "The editor UI is not ready yet.", "Close")->show();
            return;
        }
        auto selected = ui->m_selectedObjects;
        if ((!selected || selected->count() == 0) && !ui->m_selectedObject) {
            FLAlertLayer::create(
                "MationBuild",
                "Select one or more anchor objects first. Template coordinates are offsets from each selected object's position.",
                "Close"
            )->show();
            return;
        }
        auto targets = selected && selected->count() > 0
            ? selected
            : CCArray::createWithObject(ui->m_selectedObject);
        if (targets->count() > 50 || targets->count() * objects.size() > 400) {
            FLAlertLayer::create(
                "MationBuild",
                "That template would create too many objects. Select fewer anchors or use a smaller template.",
                "Close"
            )->show();
            return;
        }

        gd::string payload;
        int requested = 0;
        for (unsigned int i = 0; i < targets->count(); i++) {
            auto target = typeinfo_cast<GameObject*>(targets->objectAtIndex(i));
            if (!target) continue;
            for (auto const& item : objects) {
                auto decoration = GameObject::createWithKey(item.id);
                if (!decoration) continue;
                decoration->setPosition(target->getPosition() + CCPoint{item.x, item.y});
                decoration->setScale(item.scale);
                decoration->setRotation(item.rotation);
                payload += decoration->getSaveString(this);
                payload += ";";
                requested++;
            }
        }
        this->pasteGeneratedObjects(payload, requested);
    }

    void pasteGeneratedObjects(gd::string const& payload, int requested) {
        if (requested == 0 || payload.empty()) {
            FLAlertLayer::create("MationBuild", "No decorative objects could be created.", "Close")->show();
            return;
        }
        auto ui = m_editorUI;
        auto pasted = ui ? ui->pasteObjects(payload, false, false) : nullptr;
        auto placed = pasted ? static_cast<int>(pasted->count()) : 0;
        if (placed == 0) {
            FLAlertLayer::create(
                "MationBuild",
                "Geometry Dash could not place those objects. Try selecting different anchors or checking the template.",
                "Close"
            )->show();
            return;
        }
        auto message = fmt::format(
            "Added {} decorative accents. Use Undo to revert this pass.",
            placed
        );
        FLAlertLayer::create("MationBuild", message.c_str(), "Close")->show();
    }
};
