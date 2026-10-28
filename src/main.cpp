#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

class $modify(MationBuildMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        auto menu = this->getChildByID("bottom-menu");
        if (!menu) menu = this->getChildByID("main-menu");
        if (menu) {
            auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
            if (sprite) {
                auto button = CCMenuItemSpriteExtra::create(
                    sprite, this, menu_selector(MationBuildMenuLayer::onMationBuild)
                );
                button->setID("mationbuild-open-button");
                menu->addChild(button);
                menu->updateLayout();
            }
        }
        return true;
    }
    void onMationBuild(CCObject*) {
        FLAlertLayer::create(
            "MationBuild",
            "Auto-decoration toolkit by Solance Good.\n\nCommunity template browsing and key-gated features are under active development.",
            "Close"
        )->show();
    }
};