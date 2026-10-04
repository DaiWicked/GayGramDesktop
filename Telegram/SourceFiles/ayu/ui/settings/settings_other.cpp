// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/ui/settings/settings_other.h"

#include "lang_auto.h"
#include "ayu/ayu_lang.h"
#include "ayu/ayu_settings.h"
#include "ayu/ui/settings/ayu_builder.h"
#include "ayu/ui/settings/settings_ayu_utils.h"
#include "ayu/ui/settings/settings_main.h"
#include "boxes/abstract_box.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_text_entity.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/integration.h"
#include "ui/vertical_list.h"
#include "ui/boxes/confirm_box.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/text/text_utilities.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "window/themes/window_theme.h"

namespace Settings {

using namespace Builder;
using namespace AyuBuilder;

namespace {

void BuildOtherThings(SectionBuilder &builder) {
	const auto controller = builder.controller();

	builder.addSkip();
	builder.addButton({
		.id = u"ayu/gaygramLanguage"_q,
		.title = rpl::single(QString("GayGram 页面语言")),
		.icon = { &st::menuIconTranslate },
		.onClick = [=] {
			const auto current = Core::App().settings().readPref<bool>("ayuZhOverride", false);
			controller->show(Box([=](not_null<Ui::GenericBox*> box) {
				SingleChoiceBox(box, {
					.title = tr::lng_settings_language(),
					.options = std::vector<QString>{
						u"English"_q,
						u"简体中文"_q,
					},
					.initialSelection = current ? 1 : 0,
					.callback = [=](int index) {
						const auto enable = (index == 1);
						Core::App().settings().writePref<bool>("ayuZhOverride", enable);
						if (enable) {
							AyuLanguage::currentInstance()->applyLocalChinese();
						} else {
							AyuLanguage::currentInstance()->resetLocalChinese();
						}
						Core::Restart();
					},
				});
			}));
		},
	});
	builder.addButton({
		.id = u"ayu/registerUrlScheme"_q,
		.title = tr::ayu_RegisterURLScheme(),
		.icon = { &st::menuIconLink },
		.onClick = [=] {
			Core::Application::RegisterUrlScheme();
			controller->showToast(tr::lng_box_done(tr::now));
		},
	});
	builder.addButton({
		.id = u"ayu/resetSettings"_q,
		.title = tr::ayu_ResetSettings(),
		.icon = { &st::menuIconRestore },
		.onClick = [=] {
			controller->show(Ui::MakeConfirmBox({
				.text = tr::ayu_ResetSettingsConfirmation(tr::rich),
				.confirmed = [=](Fn<void()> &&close) {
					AyuSettings::reset();
					controller->showToast(tr::lng_box_done(tr::now));
					close();
				},
				.confirmText = tr::lng_box_yes(),
			}));
		},
	});
	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = AyuOther::Id(),
	.parentId = AyuMain::Id(),
	.title = &tr::ayu_CategoryOther,
	.icon = &st::menuIconFave,
}, [](SectionBuilder &builder) {
	builder.addSkip();
	BuildOtherThings(builder);
});

} // namespace

rpl::producer<QString> AyuOther::title() {
	return tr::ayu_CategoryOther();
}

AyuOther::AyuOther(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void AyuOther::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type AyuOtherId() {
	return AyuOther::Id();
}

} // namespace Settings
