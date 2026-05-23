#include "UI/Dialogs/ClientConfiguration/ClientConfigurationDialog.h"
#include "UI/Dialogs/ClientConfiguration/ClientPropertyEditor.h"
#include "Domain/ClientVersion.h"
#include "Services/ClientAssetDetector.h"
#include "Services/ClientVersionPersistence.h"
#include "Services/ClientVersionRegistry.h"
#include "Services/ConfigService.h"
#include <IconsFontAwesome6.h>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <format>
#include <imgui.h>
#include <map>
#include <nfd.hpp>
#include <spdlog/spdlog.h>

namespace MapEditor {
namespace UI {

namespace {

std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return s;
}

} // namespace

ClientConfigurationDialog::ClientConfigurationDialog()
    : editor_(std::make_unique<ClientPropertyEditor>()) {}

ClientConfigurationDialog::~ClientConfigurationDialog() = default;

void ClientConfigurationDialog::open(Services::ClientVersionRegistry &registry,
                                     Services::ConfigService &config) {
  registry_ = &registry;
  config_ = &config;
  is_open_ = true;
  active_version_ = 0;
  search_buf_[0] = '\0';
  search_filter_.clear();
  pending_deleted_.clear();

  editor_->setRegistry(registry_);
  editor_->setChangeCallback([this]() { runAssetDetection(); });

  if (registry_->getDefaultVersion() > 0) {
    active_version_ = registry_->getDefaultVersion();
  } else if (!registry_->getVersionsMap().empty()) {
    active_version_ = registry_->getVersionsMap().begin()->first;
  }

  populateTreeData();

  if (active_version_ != 0) {
    auto *cv = registry_->getVersion(active_version_);
    if (cv) {
      cv->backup();
      editor_->syncFromClient(*cv);
      editor_->setActiveVersion(active_version_);
    }
  }
}

void ClientConfigurationDialog::close() { is_open_ = false; }

int ClientConfigurationDialog::getMajorGroup(uint32_t version) const {
  if (version >= 10000)
    return static_cast<int>(version) / 1000;
  if (version >= 100)
    return static_cast<int>(version) / 100;
  return static_cast<int>(version) / 10;
}

bool ClientConfigurationDialog::matchesFilter(const Domain::ClientVersion &ver) const {
  if (search_filter_.empty())
    return true;
  std::string haystack = toLower(ver.getName()) + " " + toLower(ver.getDescription()) +
                         " " + std::to_string(ver.getVersion()) + " " +
                         std::to_string(ver.getOtbVersion());
  return haystack.find(search_filter_) != std::string::npos;
}

void ClientConfigurationDialog::populateTreeData() {
  tree_groups_.clear();
  std::map<int, std::vector<uint32_t>> grouped;

  for (const auto &[ver_num, cv] : registry_->getVersionsMap()) {
    if (pending_deleted_.count(ver_num))
      continue;
    if (!matchesFilter(cv))
      continue;
    grouped[getMajorGroup(ver_num)].push_back(ver_num);
  }

  for (auto &[major, vers] : grouped) {
    std::sort(vers.begin(), vers.end());
    tree_groups_.push_back(
        {major, std::format("{} ({})", (major >= 1000)
                                           ? std::format("{}.{}", major, 0)
                                       : (major >= 100)
                                           ? std::format("{}.x", major)
                                           : std::format("{}.x", major),
                            vers.size()),
         std::move(vers)});
  }
  std::sort(tree_groups_.begin(), tree_groups_.end(),
            [](const TreeGroup &a, const TreeGroup &b) {
              return a.major_version < b.major_version;
            });
}

void ClientConfigurationDialog::selectClient(uint32_t version) {
  if (version == active_version_)
    return;
  active_version_ = version;
  if (version == 0)
    return;

  auto *cv = registry_->getVersion(version);
  if (cv) {
    cv->backup();
    editor_->syncFromClient(*cv);
    editor_->setActiveVersion(version);
  }
}

void ClientConfigurationDialog::runAssetDetection() {
  if (active_version_ == 0 || !registry_)
    return;

  auto *cv = registry_->getVersion(active_version_);
  if (!cv)
    return;

  editor_->syncToClient(*cv);

  auto result = Services::ClientAssetDetector::detect(
      cv->getClientPath(), cv->getMetadataFile(), cv->getSpritesFile());

  editor_->applyDetectionResult(result);
  editor_->syncToClient(*cv);

  for (const auto &w : result.warnings)
    spdlog::warn("Asset detection: {}", w);
}

void ClientConfigurationDialog::addClient() {
  std::string new_name = "New Client";
  int counter = 1;
  bool unique = false;
  while (!unique) {
    unique = true;
    for (const auto &[num, ver] : registry_->getVersionsMap()) {
      if (ver.getName() == new_name) {
        new_name = "New Client " + std::to_string(counter++);
        unique = false;
        break;
      }
    }
  }

  uint32_t new_version = 99999;
  for (const auto &[num, _] : registry_->getVersionsMap()) {
    if (num >= new_version + 1)
      new_version = num + 1;
  }

  Domain::ClientVersion cv(new_version, new_name, 0);
  cv.setDataDirectory("1287");
  cv.setMetadataFile("Tibia.dat");
  cv.setSpritesFile("Tibia.spr");
  cv.setOtbMajor(3);
  cv.markDirty();

  registry_->addClient(cv);
  registry_->backupVersion(new_version);
  populateTreeData();
  selectClient(new_version);
}

void ClientConfigurationDialog::duplicateClient() {
  if (active_version_ == 0)
    return;
  auto *src = registry_->getVersion(active_version_);
  if (!src)
    return;

  std::string new_name = src->getName() + " (Copy)";
  uint32_t new_version = src->getVersion();
  while (registry_->getVersionsMap().count(new_version))
    ++new_version;

  Domain::ClientVersion clone(new_version, new_name, src->getOtbVersion());
  clone.setDescription(src->getDescription());
  clone.setDataDirectory(src->getDataDirectory());
  clone.setClientPath(src->getClientPath());
  clone.setOtbMajor(src->getOtbMajor());
  clone.setMapVersionsSupported(src->getMapVersionsSupported());
  clone.setDatSignature(src->getDatSignature());
  clone.setSprSignature(src->getSprSignature());
  clone.setMetadataFile(src->getMetadataFile());
  clone.setSpritesFile(src->getSpritesFile());
  clone.setTransparent(src->isTransparent());
  clone.setExtended(src->isExtended());
  clone.setFrameDurations(src->hasFrameDurations());
  clone.setFrameGroups(src->hasFrameGroups());
  clone.markDirty();

  registry_->addClient(clone);
  registry_->backupVersion(new_version);
  populateTreeData();
  selectClient(new_version);
}

void ClientConfigurationDialog::deleteClient(uint32_t version) {
  if (version == 0)
    return;
  pending_deleted_.insert(version);
  editor_->clearPropertyStates(version);
  populateTreeData();
  if (active_version_ == version) {
    active_version_ = 0;
    if (!tree_groups_.empty() && !tree_groups_[0].client_versions.empty()) {
      selectClient(tree_groups_[0].client_versions[0]);
    }
  }
}

bool ClientConfigurationDialog::validateBeforeSave() {
  std::unordered_set<std::string> names;
  for (const auto &[num, cv] : registry_->getVersionsMap()) {
    if (pending_deleted_.count(num))
      continue;
    if (cv.getName().empty()) {
      validation_error_ = std::format(
          "Client version {} has an empty name. "
          "Every client must have a name.",
          num);
      return false;
    }
    if (!names.insert(cv.getName()).second) {
      validation_error_ = std::format(
          "Duplicate client name: \"{}\". "
          "Each client must have a unique name.",
          cv.getName());
      return false;
    }
  }
  return true;
}

bool ClientConfigurationDialog::saveAll() {
  if (!registry_)
    return false;

  if (!validateBeforeSave()) {
    ImGui::OpenPopup("Validation Error");
    return false;
  }

  for (auto id : pending_deleted_)
    registry_->removeClient(id);

  Services::ClientVersionsData data;
  data.versions = registry_->getVersionsMap();
  data.otb_to_version = registry_->getOtbMapping();
  data.default_version = registry_->getDefaultVersion();

  if (!Services::ClientVersionPersistence::saveToJson(registry_->getJsonPath(), data)) {
    spdlog::error("Failed to save clients.json");
    return false;
  }

  for (const auto &[num, _] : registry_->getVersionsMap()) {
    if (auto *cv = registry_->getVersion(num))
      cv->clearDirty();
  }

  for (const auto &[num, _] : registry_->getVersionsMap())
    editor_->markAllPendingAsSaved(num);

  pending_deleted_.clear();

  if (config_) {
    registry_->savePathsToConfig(*config_);
    config_->save();
  }

  spdlog::info("Saved {} client versions", registry_->getVersionsMap().size());
  return true;
}

void ClientConfigurationDialog::discardChanges() {
  for (const auto &[num, _] : registry_->getVersionsMap()) {
    if (auto *cv = registry_->getVersion(num)) {
      if (cv->isDirty()) {
        cv->restore();
        cv->clearDirty();
      }
    }
  }

  pending_deleted_.clear();
  search_filter_.clear();
  search_buf_[0] = '\0';

  populateTreeData();

  if (active_version_ != 0) {
    auto *cv = registry_->getVersion(active_version_);
    if (cv) {
      editor_->syncFromClient(*cv);
      editor_->setActiveVersion(active_version_);
    }
  } else if (!tree_groups_.empty() && !tree_groups_[0].client_versions.empty()) {
    selectClient(tree_groups_[0].client_versions[0]);
  }
}

bool ClientConfigurationDialog::render() {
  if (!is_open_ || !registry_)
    return false;

  ImGuiIO &io = ImGui::GetIO();
  ImVec2 win_size(960, 650);
  ImVec2 win_pos((io.DisplaySize.x - win_size.x) * 0.5f,
                 (io.DisplaySize.y - win_size.y) * 0.5f);
  ImGui::SetNextWindowPos(win_pos, ImGuiCond_Appearing);
  ImGui::SetNextWindowSize(win_size, ImGuiCond_Appearing);
  ImGui::SetNextWindowSizeConstraints(ImVec2(700, 400), ImVec2(FLT_MAX, FLT_MAX));

  bool open = true;
  ImGui::Begin("Client Configuration", &open,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

  // Keyboard shortcuts
  if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    is_open_ = false;
  if (ImGui::IsKeyPressed(ImGuiKey_S) && io.KeyCtrl) {
    if (saveAll())
      populateTreeData();
  }

  renderToolbar();
  ImGui::Separator();
  renderLeftPane();
  ImGui::SameLine();
  renderRightPane();
  renderFooter();

  if (ImGui::BeginPopupModal("Validation Error", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Please fix the following before saving:");
    ImGui::TextWrapped("%s", validation_error_.c_str());
    ImGui::Spacing();
    if (ImGui::Button("OK", ImVec2(100, 0)))
      ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }

  ImGui::End();

  if (!open)
    is_open_ = false;

  return is_open_;
}

void ClientConfigurationDialog::renderToolbar() {
  if (ImGui::Button(ICON_FA_PLUS " Add", ImVec2(80, 0)))
    addClient();
  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_COPY " Duplicate", ImVec2(100, 0)))
    duplicateClient();
  ImGui::SameLine();

  bool can_delete = (active_version_ != 0);
  if (!can_delete)
    ImGui::BeginDisabled();
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.25f, 0.25f, 1.0f));
  if (ImGui::Button(ICON_FA_TRASH " Delete", ImVec2(80, 0)))
    deleteClient(active_version_);
  ImGui::PopStyleColor(2);
  if (!can_delete)
    ImGui::EndDisabled();

  ImGui::SameLine();
  float right_x = ImGui::GetContentRegionMax().x;
  ImGui::SetCursorPosX(right_x - 170);
  ImGui::SetNextItemWidth(145);
  if (ImGui::InputTextWithHint("##search", ICON_FA_MAGNIFYING_GLASS " Search...",
                               search_buf_, sizeof(search_buf_))) {
    search_filter_ = toLower(search_buf_);
    populateTreeData();
  }
  ImGui::SameLine();
  if (search_buf_[0] != '\0') {
    if (ImGui::Button(ICON_FA_XMARK "##clear_search")) {
      search_buf_[0] = '\0';
      search_filter_.clear();
      populateTreeData();
    }
  } else {
    ImGui::Dummy(ImVec2(20, 0));
  }
}

void ClientConfigurationDialog::renderFooter() {
  ImGui::Checkbox("Validate Signatures", &check_signatures_);

  float btn_x = ImGui::GetContentRegionMax().x - 300;
  ImGui::SameLine(btn_x);
  if (ImGui::Button("Save", ImVec2(80, 0))) {
    if (saveAll())
      populateTreeData();
  }
  ImGui::SameLine();
  if (ImGui::Button("Discard", ImVec2(80, 0)))
    discardChanges();
  ImGui::SameLine();
  if (ImGui::Button("Close", ImVec2(80, 0)))
    is_open_ = false;
}

void ClientConfigurationDialog::renderLeftPane() {
  ImGui::BeginChild("left_pane", ImVec2(250.0f, 0), ImGuiChildFlags_None);

  if (ImGui::BeginChild("tree_inner", ImVec2(0, -30), ImGuiChildFlags_None)) {
    if (tree_groups_.empty()) {
      ImGui::TextDisabled("No matching clients");
    }
    for (auto &group : tree_groups_) {
      ImGuiTreeNodeFlags flags =
          ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
          ImGuiTreeNodeFlags_DefaultOpen;

      if (ImGui::TreeNodeEx(group.label.c_str(), flags)) {
        for (uint32_t ver_num : group.client_versions) {
          auto *cv = registry_->getVersion(ver_num);
          if (!cv)
            continue;

          bool sel = (active_version_ == ver_num);
          std::string display =
              std::format("{} ({})", cv->getName(), cv->getVersion());
          if (cv->isDirty())
            display += " *";

          if (ImGui::Selectable(display.c_str(), sel))
            selectClient(ver_num);

          if (ImGui::BeginPopupContextItem(std::format("ctx_{}", ver_num).c_str())) {
            if (ImGui::MenuItem("Duplicate"))
              duplicateClient();
            if (ImGui::MenuItem("Delete"))
              deleteClient(ver_num);
            ImGui::EndPopup();
          }
        }
        ImGui::TreePop();
      }
    }
  }
  ImGui::EndChild();

  ImGui::PushItemWidth(-1);
  if (ImGui::BeginCombo("##default_version",
                        std::format("{} Default Client",
                                   ICON_FA_STAR).c_str())) {
    for (const auto &[num, cv] : registry_->getVersionsMap()) {
      if (pending_deleted_.count(num))
        continue;
      if (ImGui::Selectable(cv.getName().c_str(), cv.isDefault()))
        registry_->setDefaultVersion(num);
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  ImGui::EndChild();
}

void ClientConfigurationDialog::renderRightPane() {
  ImGui::BeginChild("right_pane", ImVec2(0, -30), ImGuiChildFlags_None);

  if (active_version_ == 0) {
    ImGui::TextWrapped("Select a client version from the list on the left "
                       "to edit its identity, files, compatibility, and "
                       "feature flags.");
  } else {
    editor_->render();
  }

  ImGui::EndChild();
}

} // namespace UI
} // namespace MapEditor
