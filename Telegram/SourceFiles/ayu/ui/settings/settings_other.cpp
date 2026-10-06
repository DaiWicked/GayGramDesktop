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
#include "ui/layers/layer_widget.h"
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
		.id = u"ayu/gaygramGlassStrength"_q,
		.title = rpl::single(QString("弹窗玻璃强度")),
		.icon = { &st::menuIconSettings },
		.onClick = [=] {
			const auto current = Core::App().settings().readPref<int>(
				Core::kGayGramGlassStrengthKey, 0);
			const auto initial = (current <= 0) ? 1
				: (current <= 10) ? 0
				: (current <= 35) ? 1
				: 2;
			controller->show(Box([=](not_null<Ui::GenericBox*> box) {
				SingleChoiceBox(box, {
					.title = rpl::single(QString("弹窗玻璃强度")),
					.options = std::vector<QString>{
						u"弱（6px）"_q,
						u"中（20px，默认）"_q,
						u"强（56px）"_q,
					},
					.initialSelection = initial,
					.callback = [=](int index) {
						const auto radius = (index == 0) ? 6
							: (index == 1) ? 20
							: 56;
						Core::App().settings().writePref<int>(
							Core::kGayGramGlassStrengthKey, radius);
						Ui::SetGlassBlurRadius(radius);
						controller->showToast(tr::lng_box_done(tr::now));
					},
				});
			}));
		},
	});
	builder.addButton({
		.id = u"ayu/gaygramSidebarWidth"_q,
		.title = rpl::single(QString("侧栏宽度")),
		.icon = { &st::menuIconChats },
		.onClick = [=] {
			const auto current = Core::App().settings().readPref<int>(
				Core::kGayGramSidebarWidthKey, 0);
			const auto initial = (current <= 0) ? 1
				: (current <= 56) ? 0
				: (current <= 72) ? 1
				: 2;
			controller->show(Box([=](not_null<Ui::GenericBox*> box) {
				SingleChoiceBox(box, {
					.title = rpl::single(QString("侧栏宽度")),
					.options = std::vector<QString>{
						u"窄（56px）"_q,
						u"默认（72px）"_q,
						u"宽（80px）"_q,
					},
					.initialSelection = initial,
					.callback = [=](int index) {
						const auto width = (index == 0) ? 56
							: (index == 1) ? 0
							: 80;
						Core::App().settings().writePref<int>(
							Core::kGayGramSidebarWidthKey, width);
						controller->showToast(tr::lng_box_done(tr::now));
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
