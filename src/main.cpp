#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/utils/web.hpp>
#include <algorithm>
#include <cctype>

using namespace geode::prelude;

class $modify(MationBuildEditorLayer, LevelEditorLayer) {
    struct Fields {
        async::TaskHolder<web::WebResponse> m_keyCheck;
    };

    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) return false;
        if (noUI) return true;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        if (!menu) return true;
        menu->setID("solance.mationbuild/editor-menu");
        menu->setPosition({winSize.width - 72.f, winSize.height - 42.f});
        menu->setZOrder(1000);

        auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        if (!sprite) return true;
        auto button = CCMenuItemSpriteExtra::create(
            sprite, this, menu_selector(MationBuildEditorLayer::onMationBuild)
        );
        if (!button) return true;
        button->setID("solance.mationbuild/auto-decorate");
        menu->addChild(button);
        this->addChild(menu, 1000);
        return true;
    }

    void onMationBuild(CCObject*) {
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
        // The key has been restricted to alphanumeric characters and hyphens before JSON insertion.
        req.bodyString(fmt::format(R"({{"action":"verify","key":"{}"}})", key));

        m_fields->m_keyCheck.spawn(
            "Verify MationBuild access key",
            req.post("https://gfgmknlxzdfnjdpxetdz.supabase.co/functions/v1/mationbuild-api"),
            [this](web::WebResponse response) {
                auto body = response.string().unwrapOr("");
                if (!response.ok() || body.find("\"valid\":true") == std::string::npos) {
                    FLAlertLayer::create(
                        "MationBuild",
                        "Your access key could not be verified. Check that it is active and that you are online.",
                        "Close"
                    )->show();
                    return;
                }
                this->applyAutoDecoration();
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

        gd::string payload;
        int requested = 0;
        for (unsigned int i = 0; i < targets->count(); i++) {
            auto target = typeinfo_cast<GameObject*>(targets->objectAtIndex(i));
            if (!target) continue;

            // Geometry Dash 2.2081 object IDs 1764–1767 are tiny visual accents:
            // Circle, Cross, Triangle, and Rectangle.
            const struct Accent {
                int id;
                CCPoint offset;
                float scale;
            } accents[] = {
                {1764, {-10.f, 10.f}, 0.35f},
                {1765, { 10.f,-10.f}, 0.35f},
                {1766, {-10.f,-10.f}, 0.35f},
                {1767, { 10.f, 10.f}, 0.35f}
            };

            for (auto const& accent : accents) {
                auto decoration = GameObject::createWithKey(accent.id);
                if (!decoration) continue;
                decoration->setPosition(target->getPosition() + accent.offset);
                decoration->setScale(accent.scale);
                payload += decoration->getSaveString(this);
                payload += ";";
                requested++;
            }
        }

        if (requested == 0 || payload.empty()) {
            FLAlertLayer::create("MationBuild", "No decorative objects could be created for that selection.", "Close")->show();
            return;
        }

        // Use Geometry Dash's normal editor paste path so additions participate in undo.
        auto pasted = ui->pasteObjects(payload, false, false);
        auto placed = pasted ? static_cast<int>(pasted->count()) : 0;
        auto message = fmt::format(
            "Added {} small accents around your selection. Use Undo to revert this pass.",
            placed
        );
        FLAlertLayer::create("MationBuild", message.c_str(), "Close")->show();
    }
};
