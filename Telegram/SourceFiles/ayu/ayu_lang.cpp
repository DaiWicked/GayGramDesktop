// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ayu/ayu_lang.h"

#include "qjsondocument.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_instance.h"
#include "storage/localstorage.h"

#include <QDir>
#include <QFile>

// hard-coded languages
std::map<QString, QString> langMapping = {
	{"pt-br", "pt"},
	{"zh-hans-beta", "zh-hans"},
	{"zh-hant-beta", "zh-hant"},
	{"zh-hans-raw", "zh-hans"},
	{"zh-hant-raw", "zh-hant"},
};

constexpr auto postfixes = {
	"zero",
	"one",
	"two",
	"few",
	"many",
	"other"
};

AyuLanguage *AyuLanguage::instance = nullptr;

AyuLanguage::AyuLanguage() = default;

void AyuLanguage::init() {
	if (!instance) instance = new AyuLanguage;
	instance->loadCachedLanguage();
}

AyuLanguage *AyuLanguage::currentInstance() {
	return instance;
}

QString AyuLanguage::getCacheDir() const {
	return cWorkingDir() + u"tdata/ayu/languages/"_q;
}

QString AyuLanguage::getCachePath(const QString &langId) const {
	return getCacheDir() + langId + u".json"_q;
}

void AyuLanguage::loadCachedLanguage() {
	return;
}

void AyuLanguage::saveCachedLanguage(const QByteArray &json, const QString &langId) {
	const auto cacheDir = getCacheDir();
	QDir().mkpath(cacheDir);

	const auto cachePath = getCachePath(langId);
	QFile file(cachePath);
	if (file.open(QIODevice::WriteOnly)) {
		file.write(json);
		file.close();
		LOG(("Cached AyuGram language: %1").arg(langId));
	}
}

void AyuLanguage::fetchLanguage(const QString &id, const QString &baseId) {
	return;
}

void AyuLanguage::fetchFinished() {
	if (!_chkReply) return;

	QString langPackBaseId = Lang::GetInstance().baseId();
	QString langPackId = Lang::GetInstance().id();
	auto statusCode = _chkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	if (statusCode == 404 && !langPackId.isEmpty() && !langPackBaseId.isEmpty() && !needFallback) {
		LOG(("AyuGram Language not found! Fallback to main language: %1...").arg(langPackBaseId));
		needFallback = true;
		_chkReply->disconnect();
		fetchLanguage("", langPackBaseId);
	} else {
		const auto result = _chkReply->readAll().trimmed();
		QJsonParseError error{};
		const auto doc = QJsonDocument::fromJson(result, &error);
		if (error.error == QJsonParseError::NoError) {
			saveCachedLanguage(result, _currentLangId);
			applyLanguageJson(doc);
		} else {
			LOG(("Incorrect language JSON File."));
		}

		_chkReply = nullptr;
	}
}

void AyuLanguage::fetchError(QNetworkReply::NetworkError e) {
	LOG(("Network error: %1").arg(e));

	if (e == QNetworkReply::NetworkError::ContentNotFoundError) {
		const auto baseId = Lang::GetInstance().baseId();
		const auto id = Lang::GetInstance().id();

		if (!id.isEmpty() && !baseId.isEmpty() && !needFallback) {
			LOG(("AyuGram Language not found! Fallback to main language: %1...").arg(baseId));
			needFallback = true;
			_chkReply->disconnect();
			fetchLanguage("", baseId);
		} else {
			LOG(("AyuGram Language not found!"));
			_chkReply = nullptr;
		}
	}
}

void AyuLanguage::applyLanguageJson(QJsonDocument doc) {
	const auto json = doc.object();
	for (const QString &brokenKey : json.keys()) {
		auto key = qsl("ayu_") + brokenKey;
		auto val = json.value(brokenKey).toString().replace(qsl("&amp;"), qsl("&"));

		if (key.endsWith("_Android")) {
			continue;
		}

		for (const auto &postfix : postfixes) {
			if (key.endsWith(qsl("_") + postfix)) {
				key = key.replace(qsl("_") + postfix, qsl("#") + postfix);
				break;
			}
		}

		if (key.endsWith("_PC")) {
			key = key.replace("_PC", "");
		}

		if (val.contains(qsl("%1$d")) && !val.contains(qsl("%2$d"))) {
			val = val.replace(qsl("%1$d"), qsl("{count}"));
		} else if (val.contains(qsl("%1$d")) && val.contains(qsl("%2$d"))) {
			val = val.replace(qsl("%1$d"), qsl("{count1}")).replace(qsl("%2$d"), qsl("{count2}"));
		} else if (val.contains(qsl("%1$s")) && !val.contains(qsl("%2$s"))) {
			val = val.replace(qsl("%1$s"), qsl("{item}"));
		} else if (val.contains(qsl("%1$s")) && val.contains(qsl("%2$s"))) {
			val = val.replace(qsl("%1$s"), qsl("{item1}")).replace(qsl("%2$s"), qsl("{item2}"));
		}

		Lang::GetInstance().resetValue(key.toUtf8());
		Lang::GetInstance().applyValue(key.toUtf8(), val.toUtf8());
	}
	Lang::GetInstance().updatePluralRules();
}

namespace {

const std::map<QString, QString> kChineseTranslations = {
	{"ayu_AyuPreferences", "GayGram 设置"},
	{"ayu_CategoriesHeader", "分类"},
	{"ayu_CategoryGeneral", "通用"},
	{"ayu_CategoryAppearance", "外观"},
	{"ayu_CategoryChats", "聊天"},
	{"ayu_CategoryOther", "其他"},
	{"ayu_CategoryFilters", "过滤器"},
	{"ayu_CategoryGhostMode", "隐身模式"},
	{"ayu_CategorySpy", "间谍模式"},
	{"ayu_CategoryCustomization", "自定义"},
	{"ayu_SettingsDescription", "GayGram Desktop\n\n海内存知己，天涯若比邻。\nA bosom friend afar brings a distant land near."},
	{"ayu_LinksHeader", "链接"},
	{"ayu_LinksChannel", "频道"},
	{"ayu_LinksDocumentation", "文档"},
	{"ayu_ResetSettings", "重置设置"},
	{"ayu_ResetSettingsConfirmation", "确定要重置所有 GayGram 设置吗？"},
	{"ayu_RegisterURLScheme", "注册 URL 协议"},
	{"ayu_GhostEssentialsHeader", "隐身功能"},
	{"ayu_DontReadMessages", "不标记已读消息"},
	{"ayu_DontReadStories", "不标记已读动态"},
	{"ayu_DontSendOnlinePackets", "不发送在线状态"},
	{"ayu_DontSendUploadProgress", "不发送输入状态"},
	{"ayu_SendOfflinePacketAfterOnline", "上线后自动离线"},
	{"ayu_MarkReadAfterAction", "交互后标记已读"},
	{"ayu_MarkReadAfterActionDescription", "回复、反应或删除消息后标记为已读"},
	{"ayu_UseScheduledMessages", "定时消息"},
	{"ayu_UseScheduledMessagesDescription", "使用定时消息功能"},
	{"ayu_SendWithoutSoundByDefault", "默认静默发送"},
	{"ayu_SendWithoutSoundByDefaultDescription", "默认以无声音方式发送消息"},
	{"ayu_SendWithoutSoundByDefaultAlways", "始终"},
	{"ayu_SendWithoutSoundByDefaultNever", "从不"},
	{"ayu_SendWithoutSoundByDefaultInGhostMode", "仅隐身模式"},
	{"ayu_SuggestGhostModeBeforeViewingStory", "查看动态前提示隐身"},
	{"ayu_SuggestGhostModeBeforeViewingStoryDescription", "查看动态前提示启用隐身模式"},
	{"ayu_GhostModeToggle", "隐身模式"},
	{"ayu_GhostModeGlobalSettings", "全局设置"},
	{"ayu_GhostModeOptionShiftDescription", "按住 Shift 点击可单独设置"},
	{"ayu_GhostModeSwitchedToGlobalSettings", "已切换到全局设置"},
	{"ayu_GhostModeSwitchedToIndividualSettings", "已切换到单独设置"},
	{"ayu_SpyEssentialsHeader", "间谍功能"},
	{"ayu_SaveDeletedMessages", "保存已删除消息"},
	{"ayu_SaveMessagesHistory", "保存编辑历史"},
	{"ayu_LReadMessages", "本地已读"},
	{"ayu_SReadMessages", "云端已读"},
	{"ayu_DisableAds", "禁用广告"},
	{"ayu_DisableStories", "禁用动态"},
	{"ayu_DisableOpenLinkWarning", "禁用打开链接警告"},
	{"ayu_DisableCustomBackgrounds", "禁用自定义背景"},
	{"ayu_HidePremiumStatuses", "隐藏高级会员状态"},
	{"ayu_DisableNotificationsDelay", "禁用通知延迟"},
	{"ayu_LocalPremium", "本地高级会员"},
	{"ayu_HideReactions", "隐藏反应"},
	{"ayu_HideReactionsInChannels", "频道中隐藏反应"},
	{"ayu_HideReactionsInGroups", "群组中隐藏反应"},
	{"ayu_HideReactionsInPrivateChats", "私聊中隐藏反应"},
	{"ayu_QuickAdminShortcuts", "快速管理快捷方式"},
	{"ayu_DisableGreetingSticker", "禁用问候贴纸"},
	{"ayu_TranslationProvider", "翻译提供商"},
	{"ayu_DeletedMarkText", "已删除标记"},
	{"ayu_EditedMarkText", "已编辑标记"},
	{"ayu_ReplaceMarksWithIcons", "用图标替换标记"},
	{"ayu_SemiTransparentDeletedMessages", "半透明已删除消息"},
	{"ayu_AvatarCorners", "头像圆角"},
	{"ayu_AvatarCornersCircle", "圆形"},
	{"ayu_AvatarCornersSquare", "方形"},
	{"ayu_SingleCornerRadius", "单角圆角"},
	{"ayu_SingleCornerRadiusDescription", "仅使用一个角的圆角值"},
	{"ayu_HideAllChats", "隐藏\"所有聊天\"标签"},
	{"ayu_ChannelBottomButton", "频道底部按钮"},
	{"ayu_ChannelBottomButtonDiscuss", "讨论"},
	{"ayu_ChannelBottomButtonHide", "隐藏"},
	{"ayu_ChannelBottomButtonMute", "静音"},
	{"ayu_SettingsShowID", "显示用户 ID"},
	{"ayu_SettingsShowID_Hide", "隐藏"},
	{"ayu_SettingsUnlimitedRecentStickers", "无限最近贴纸"},
	{"ayu_MaterialSwitches", "MD3 开关样式"},
	{"ayu_RemoveMessageTail", "移除消息尾巴"},
	{"ayu_HideShareButton", "隐藏侧边\"分享\"按钮"},
	{"ayu_ChatFoldersHeader", "聊天文件夹"},
	{"ayu_ContextMenuElementsHeader", "右键菜单元素"},
	{"ayu_MessageFieldElementsHeader", "输入框元素"},
	{"ayu_MessageFieldPopupsHeader", "输入框弹窗"},
	{"ayu_MessageFieldElementAttach", "附件"},
	{"ayu_MessageFieldElementCommands", "命令"},
	{"ayu_MessageFieldElementEmoji", "表情"},
	{"ayu_MessageFieldElementTTL", "自毁消息"},
	{"ayu_MessageFieldElementVoice", "语音"},
	{"ayu_DrawerElementsHeader", "侧边栏元素"},
	{"ayu_TrayElementsHeader", "托盘元素"},
	{"ayu_MessageBubbleRadius", "消息气泡圆角"},
	{"ayu_SettingsWideMultiplier", "宽消息倍数"},
	{"ayu_SettingsWideMultiplierDescription", "宽消息的宽度倍数"},
	{"ayu_SettingsSpoofWebviewAsAndroid", "伪装为安卓客户端"},
	{"ayu_SettingsBiggerWindow", "更大窗口"},
	{"ayu_SettingsIncreaseWebviewWidth", "增加网页宽度"},
	{"ayu_SettingsIncreaseWebviewHeight", "增加网页高度"},
	{"ayu_SettingsShowMessageSeconds", "显示消息秒数"},
	{"ayu_SettingsShowMessageShot", "消息截图"},
	{"ayu_SettingsShowMessageShotDescription", "启用消息截图功能"},
	{"ayu_SettingsContextMenuTitle", "右键菜单"},
	{"ayu_SettingsContextMenuDescription", "自定义右键菜单显示内容"},
	{"ayu_SettingsContextMenuItemShown", "显示"},
	{"ayu_SettingsContextMenuItemHidden", "隐藏"},
	{"ayu_SettingsContextMenuItemExtended", "扩展"},
	{"ayu_SettingsContextMenuReactionsPanel", "反应面板"},
	{"ayu_SettingsContextMenuViewsPanel", "查看面板"},
	{"ayu_RegexFilters", "消息过滤器"},
	{"ayu_RegexFiltersHeader", "正则过滤器"},
	{"ayu_RegexFiltersAdd", "添加过滤器"},
	{"ayu_RegexFiltersEdit", "编辑过滤器"},
	{"ayu_RegexFiltersEnable", "启用过滤器"},
	{"ayu_RegexFiltersEnableSharedInChats", "在聊天中启用共享过滤器"},
	{"ayu_RegexFiltersExcluded", "排除的聊天"},
	{"ayu_RegexFiltersExcludedAmount#one", "已排除 {count} 个"},
	{"ayu_RegexFiltersExcludedAmount#other", "已排除 {count} 个"},
	{"ayu_RegexFiltersListEmpty", "过滤器列表为空"},
	{"ayu_RegexFiltersPlaceholder", "输入正则表达式"},
	{"ayu_RegexFiltersShared", "共享过滤器"},
	{"ayu_RegexFiltersAmount#one", "{count} 个过滤器"},
	{"ayu_RegexFiltersAmount#other", "{count} 个过滤器"},
	{"ayu_RegexFilterQuickAdd", "快速添加"},
	{"ayu_RegexFilterBulletinText", "公告文本"},
	{"ayu_RegexFilterBulletinActionText", "公告操作"},
	{"ayu_FiltersShadowBan", "影子封禁"},
	{"ayu_FiltersHideFromBlocked", "对已屏蔽用户隐藏"},
	{"ayu_FiltersMenuClear", "清除过滤器"},
	{"ayu_FiltersMenuExport", "导出过滤器"},
	{"ayu_FiltersMenuImport", "导入过滤器"},
	{"ayu_FiltersMenuSelectChat", "选择聊天"},
	{"ayu_FiltersClearPopupText", "确定要清除所有过滤器吗？"},
	{"ayu_FiltersClearPopupActionText", "清除"},
	{"ayu_EnableExpression", "启用过滤器"},
	{"ayu_CaseInsensitiveExpression", "不区分大小写"},
	{"ayu_ReversedExpression", "反向匹配"},
	{"ayu_FilterZalgo", "过滤 Zalgo 字符"},
	{"ayu_ImproveLinkPreviews", "改进链接预览"},
	{"ayu_ShowOnlyAddedEmojisAndStickers", "仅显示已添加表情和贴纸"},
	{"ayu_AppIconHeader", "应用图标"},
	{"ayu_ConfirmationsTitle", "发送确认"},
	{"ayu_StickerConfirmation", "贴纸"},
	{"ayu_GIFConfirmation", "GIF"},
	{"ayu_VoiceConfirmation", "语音消息"},
	{"ayu_RoundConfirmation", "圆形视频"},
	{"ayu_MessageDetailsPC", "详情"},
	{"ayu_EnableGhostModeTray", "启用隐身模式"},
	{"ayu_StreamerModeToggle", "主播模式"},
	{"ayu_EnableStreamerModeTray", "启用主播模式"},
	{"ayu_UserMessagesMenuText", "用户消息"},
	{"ayu_SimpleQuotesAndReplies", "禁用彩色回复"},
	{"ayu_DisableSimilarChannels", "禁用相似频道"},
	{"ayu_CollapseSimilarChannels", "折叠相似频道"},
	{"ayu_HideSimilarChannelsTab", "隐藏相似频道标签"},
	{"ayu_MonospaceFont", "等宽字体"},
	{"ayu_FontDefault", "默认"},
	{"ayu_ContextHideMessage", "隐藏"},
	{"ayu_RepeatMessage", "重复消息"},
	{"ayu_MessageSavingOtherHeader", "消息保存"},
	{"ayu_MessageSavingSaveForBots", "为机器人保存"},
	{"ayu_HideNotificationBadge", "隐藏通知徽章"},
	{"ayu_HideNotificationBadgeDescription", "隐藏任务栏通知徽章"},
	{"ayu_HideNotificationCounters", "隐藏通知计数"},
};

} // namespace

void AyuLanguage::applyLocalChinese() {
	for (const auto &[key, value] : kChineseTranslations) {
		Lang::GetInstance().resetValue(key.toUtf8());
		Lang::GetInstance().applyValue(key.toUtf8(), value.toUtf8());
	}
	Lang::GetInstance().updatePluralRules();
	Local::writeLangPack();
}

void AyuLanguage::resetLocalChinese() {
	for (const auto &[key, value] : kChineseTranslations) {
		Lang::GetInstance().resetValue(key.toUtf8());
	}
	Lang::GetInstance().updatePluralRules();
	Local::writeLangPack();
}
