#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

class MationBuildPopup final : public FLAlertLayer {
protected:
    bool init() {
        if (!FLAlertLayer::init(360.f, 220.f, "bigFont.fnt", "MationBuild", 0.f, 0.f, 300.f))
            return false;
        auto size = CCDirector::sharedDirector()->getWinSize();
        auto body = CCLabelBMFont::create(
            "Auto-decoration toolkit\nCommunity template browser is in development.\nManage access keys on the project website.",
            "bigFont.fnt"
        );
        body->setAlignment(kCCTextAlignmentCenter);
        body->setWidth(300.f);
        body->setScale(0.42f);
        body->setPosition({size.width / 2.f, size.height / 2.f + 5.f});
        m_mainLayer->addChild(body);
        return true;
    }
public:
    static MationBuildPopup* create() {
        auto ret = new MationBuildPopup();
        if (ret && ret->init()) { ret->autorelease(); return ret; }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

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
    void onMationBuild(CCObject*) { MationBuildPopup::create()->show(); }
};