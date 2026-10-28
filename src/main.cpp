#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

// A conservative first auto-decoration pass: users explicitly select objects,
// then MationBuild adds small non-trigger decorative accents around that selection.
// It never scans or modifies the entire level and uses the editor's paste/undo path.
class $modify(MationBuildEditorLayer, LevelEditorLayer) {
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
        auto ui = m_editorUI;
        if (!ui) {
            FLAlertLayer::create("MationBuild", "The editor UI is not ready yet.", "Close")->show();
            return;
        }

        auto selected = ui->m_selectedObjects;
        if ((!selected || selected->count() == 0) && !ui->m_selectedObject) {
            FLAlertLayer::create(
                "MationBuild",
                "Select one or more objects first. MationBuild will add small decorative accents only around your selection.",
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
                "Please select 100 objects or fewer for this pass. This limit helps prevent accidental level clutter.",
                "Close"
            )->show();
            return;
        }

        gd::string payload;
        int requested = 0;
        for (unsigned int i = 0; i < targets->count(); i++) {
            auto target = typeinfo_cast<GameObject*>(targets->objectAtIndex(i));
            if (!target) continue;

            // IDs 1764 and 1765 are the small Circle and Cross decoration objects
            // in Geometry Dash 2.2081. They are visual accents, not gameplay triggers.
            const struct Accent {
                int id;
                CCPoint offset;
                float scale;
            } accents[] = {
                {1764, {-10.f, 10.f}, 0.35f},
                {1765, { 10.f,-10.f}, 0.35f}
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
            "Added {} small accents around your selection. Use Undo if you want to revert this pass.",
            placed
        );
        FLAlertLayer::create("MationBuild", message.c_str(), "Close")->show();
    }
};
