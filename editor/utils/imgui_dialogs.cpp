#include "utils/imgui_dialogs.h"

#include "imgui.h"
#include <deque>

static ImGuiKey DefaultAcceptKey = ImGuiKey_Enter;

namespace ImGui
{
    void SetDialogAcceptKey(ImGuiKey key)
    {
        DefaultAcceptKey = key;
    }

    DialogResult DialogButtons(bool allowAccept, const char* accept, const char* cancel, const char* decline)
    {
        ImVec2 size = ImGui::GetContentRegionAvail();

        int buttons = 1;
        if (decline != nullptr)
            buttons++;
        if (cancel != nullptr)
            buttons++;

        float textWidth = ImGui::CalcTextSize(accept).x;
        if (decline != nullptr)
            textWidth += ImGui::CalcTextSize(decline).x;
        if (cancel != nullptr)
            textWidth += ImGui::CalcTextSize(cancel).x;

        auto& style = ImGui::GetStyle();

        float width = (buttons - 1) * style.ItemInnerSpacing.x;
        width += style.ItemSpacing.x * 2;
        width += textWidth;

        DialogResult result = DialogResult::None;

        ImGui::SetCursorPosX(size.x - width);

        if (ImGui::ConditionalButton(accept, allowAccept) ||  (allowAccept && ImGui::IsKeyPressed(DefaultAcceptKey)))
            result = DialogResult::Accept;

        if (allowAccept)
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();

            float offset = 2;
            min.x += offset;
            min.y += offset;
            max.x -= offset;
            max.y -= offset;

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            drawList->AddRect(min, max, ImGui::GetColorU32(style.Colors[ImGuiCol_NavWindowingHighlight]));
        }

        if (decline != nullptr)
        {
            ImGui::SameLine(0, style.ItemSpacing.x);
            if (ImGui::Button(decline))
                result = DialogResult::Decline;
        }

        if (cancel != nullptr)
        {
            ImGui::SameLine(0, style.ItemSpacing.x);
            if (ImGui::Button(cancel))
                result = DialogResult::Cancel;
        }

        return result;
    }

    struct DialogInfo
    {
        DialogBox* Dialog = nullptr;
        std::function<void(DialogBox*)> DestoryFunc = nullptr;
    };

    std::deque<DialogInfo> PendingDialogs;
    DialogBox* ActiveDialog = nullptr;

    DialogBox* ShowDialog(DialogBox* dialog, std::function<void(DialogBox*)> destoryFunction)
    {
        if (dialog == nullptr)
            return nullptr;

        PendingDialogs.emplace_back(DialogInfo{ dialog,destoryFunction });

        // TODO, support multiple dialogs and do create here

        return dialog;
    }

    size_t UpdateDialogs()
    {
        if (!PendingDialogs.empty())
        {
            if (ActiveDialog == nullptr)
            {
                ActiveDialog = PendingDialogs[0].Dialog;

                ActiveDialog->ImGuiName = ActiveDialog->Icon;
                if (!ActiveDialog->Icon.empty())
                    ActiveDialog->ImGuiName += " ";
                ActiveDialog->ImGuiName += ActiveDialog->GetDisplayName();
                ActiveDialog->ImGuiName += "###Dialog";
                ActiveDialog->ImGuiName += ActiveDialog->GetDisplayName();

                ActiveDialog->OnCreate();

                ImGuiPopupFlags flags = ImGuiPopupFlags_None;
                ImGui::OpenPopup(ActiveDialog->ImGuiName.c_str(), flags);
            }

            if (ActiveDialog != nullptr)
            {
                bool destoryDialog = false;

                DialogResult result = DialogResult::None;
                bool open = true;

                ImGui::SetNextWindowSize(ActiveDialog->InitalSize);

                if (ImGui::BeginPopupModal(ActiveDialog->ImGuiName.c_str(), &open, ImGuiWindowFlags_NoResize))
                {
                    ImVec2 size(-1, ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y - ImGui::GetStyle().ItemSpacing.y);
                    if (ImGui::BeginChild("DialogContent", size))
                    {
                        ActiveDialog->OnShow();
                        ImGui::EndChild();
                    }

                    result = ImGui::DialogButtons(ActiveDialog->AllowAccept(), ActiveDialog->AcceptButton, ActiveDialog->CancelButton, ActiveDialog->DeclineButton);

                    // TODO, to support multiple dialogs, recurse here

                    if (result != DialogResult::None)
                    {
                        ActiveDialog->OnResult(result);
                        ImGui::CloseCurrentPopup();
                        destoryDialog = true;
                    }

                    ImGui::EndPopup();
                }

                if (!open && result == DialogResult::None)
                {
                    ActiveDialog->OnResult(DialogResult::Cancel);
                    destoryDialog = true;
                }

                if (destoryDialog)
                {
                    ActiveDialog->OnClose();

                    if (PendingDialogs[0].DestoryFunc != nullptr)
                        PendingDialogs[0].DestoryFunc(ActiveDialog);
                    else
                        delete(ActiveDialog);

                    PendingDialogs.erase(PendingDialogs.begin());
                    ActiveDialog = nullptr;
                }
            }
        }

        return PendingDialogs.size();
    }
}