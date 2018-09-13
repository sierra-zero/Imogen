#pragma once

#include "Nodes.h"
#include "Evaluation.h"
#include "curve.h"

struct TileNodeEditGraphDelegate : public NodeGraphDelegate
{
	TileNodeEditGraphDelegate(Evaluation& evaluation) : mEvaluation(evaluation)
	{}

	Evaluation& mEvaluation;
	struct ImogenNode
	{
		size_t mType;
		size_t mEvaluationTarget;
		void *mParameters;
		size_t mParametersSize;
	};

	std::vector<ImogenNode> mNodes;

	enum ConTypes
	{
		Con_Float,
		Con_Float2,
		Con_Float3,
		Con_Float4,
		Con_Color4,
		Con_Int,
		Con_Ramp,
		Con_Angle,
		Con_Angle2,
		Con_Angle3,
		Con_Angle4,
		Con_Enum,
		Con_Structure,
		Con_Any,
	};
	virtual unsigned char *GetParamBlock(int index, size_t& paramBlockSize)
	{
		const ImogenNode & node = mNodes[index];
		paramBlockSize = ComputeParamMemSize(node.mType);
		return (unsigned char*)node.mParameters;
	}
	virtual void SetParamBlock(int index, unsigned char* parameters)
	{
		const ImogenNode & node = mNodes[index];
		memcpy(node.mParameters, parameters, ComputeParamMemSize(node.mType));
		mEvaluation.SetEvaluationParameters(node.mEvaluationTarget, parameters, node.mParametersSize);
	}

	virtual bool AuthorizeConnexion(int typeA, int typeB)
	{
		return true;
	}

	virtual unsigned int GetNodeTexture(size_t index)
	{
		return mEvaluation.GetEvaluationTexture(mNodes[index].mEvaluationTarget);
	}
	virtual void AddNode(size_t type)
	{
		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);

		size_t index = mNodes.size();
		ImogenNode node;
		node.mEvaluationTarget = mEvaluation.AddEvaluationTarget(type, metaNodes[type].mName);
		node.mType = type;
		size_t paramsSize = ComputeParamMemSize(type);
		node.mParameters = malloc(paramsSize);
		node.mParametersSize = paramsSize;
		memset(node.mParameters, 0, paramsSize);
		mNodes.push_back(node);

		mEvaluation.SetEvaluationParameters(node.mEvaluationTarget, node.mParameters, node.mParametersSize);
	}

	void AddLink(int InputIdx, int InputSlot, int OutputIdx, int OutputSlot)
	{
		mEvaluation.AddEvaluationInput(OutputIdx, OutputSlot, InputIdx);
	}

	virtual void DelLink(int index, int slot)
	{
		mEvaluation.DelEvaluationInput(index, slot);
	}

	virtual void DeleteNode(size_t index)
	{
		mEvaluation.DelEvaluationTarget(index);
		free(mNodes[index].mParameters);
		mNodes.erase(mNodes.begin() + index);
		for (auto& node : mNodes)
		{
			if (node.mEvaluationTarget > index)
				node.mEvaluationTarget--;
		}
	}
	virtual const MetaNode* GetMetaNodes(int &metaNodeCount)
	{
		static const uint32_t hcTransform = IM_COL32(200, 200, 200, 255);
		static const uint32_t hcGenerator = IM_COL32(150, 200, 150, 255);
		static const uint32_t hcMaterial = IM_COL32(150, 150, 200, 255);
		static const uint32_t hcBlend = IM_COL32(200, 150, 150, 255);
		static const uint32_t hcFilter = IM_COL32(200, 200, 150, 255);
		static const uint32_t hcNoise = IM_COL32(150, 250, 150, 255);

		metaNodeCount = 21;

		static const MetaNode metaNodes[21] = {
			{
				"Circle", hcGenerator
				,{ {} }
			,{ { "", (int)Con_Float4 } }
			,{ { "Radius", (int)Con_Float, 0.f,1.f,0.f,0.f },{ "T", (int)Con_Float } }
			}
			,
			{
				"Transform", hcTransform
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Translate", (int)Con_Float2, 1.f,0.f,1.f,0.f, true },{ "Rotation", (int)Con_Angle },{ "Scale", (int)Con_Float } }
			}
			,
			{
				"Square", hcGenerator
				,{ { } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Width", (int)Con_Float } }
			}
			,
			{
				"Checker", hcGenerator
				,{ {} }
			,{ { "", (int)Con_Float4 } }
			,{  }
			}
			,
			{
				"Sine", hcGenerator
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Frequency", (int)Con_Float },{ "Angle", (int)Con_Angle } }
			}

			,
			{
				"SmoothStep", hcFilter
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Low", (int)Con_Float },{ "High", (int)Con_Float } }
			}

			,
			{
				"Pixelize", hcTransform
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "scale", (int)Con_Float } }
			}


			,
			{
				"Blur", hcFilter
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "angle", (int)Con_Float },{ "strength", (int)Con_Float } }
			}

			,
			{
				"NormalMap", hcFilter
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "spread", (int)Con_Float } }
			}

			,
			{
				"LambertMaterial", hcMaterial
				,{ { "Diffuse", (int)Con_Float4 },{ "Normal", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "view", (int)Con_Float2, 1.f,0.f,0.f,1.f } }
			}

			,
			{
				"MADD", hcBlend
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Mul Color", (int)Con_Color4 }, {"Add Color", (int)Con_Color4} }
			}
			
			,
			{
				"Hexagon", hcGenerator
				,{  }
			,{ { "", (int)Con_Float4 } }
			,{  }
			}

			,
			{
				"Blend", hcBlend
				,{ { "", (int)Con_Float4 },{ "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ {"A", (int)Con_Float4 },{ "B", (int)Con_Float4 },{ "Operation", (int)Con_Enum, 0.f,0.f,0.f,0.f, false, "Add\0Mul\0Min\0Max\0" } }
			}

			,
			{
				"Invert", hcFilter
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{}
			}

			,
			{
				"CircleSplatter", hcGenerator
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Distance", (int)Con_Float2 },{ "Radius", (int)Con_Float2 },{ "Angle", (int)Con_Angle2 },{ "Count", (int)Con_Float } }
			}

			,
			{
				"Ramp", hcFilter
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Ramp", (int)Con_Ramp } }
			}

			,
			{
				"Tile", hcTransform
				,{ { "", (int)Con_Float4 } }
			,{ { "", (int)Con_Float4 } }
			,{ { "Scale", (int)Con_Float },{ "Offset 0", (int)Con_Float2 },{ "Offset 1", (int)Con_Float2 },{ "Overlap", (int)Con_Float2 } }
			}

				,
				{
					"Color", hcGenerator
					,{  }
				,{ { "", (int)Con_Float4 } }
				,{ { "Color", (int)Con_Color4 } }
				}


				,
				{
					"NormalMapBlending", hcBlend
					,{ { "", (int)Con_Float4 },{ "", (int)Con_Float4 } }
				,{ { "Out", (int)Con_Float4 } }
				,{ { "Technique", (int)Con_Enum, 0.f,0.f,0.f,0.f, false, "RNM\0Partial Derivatives\0Whiteout\0UDN\0Unity\0Linear\0Overlay\0" } }
				}

				,
				{
					"iqnoise", hcNoise
					,{ }
				,{ { "", (int)Con_Float4 } }
				,{ { "Size", (int)Con_Float }, { "U", (int)Con_Float, 0.f,1.f,0.f,0.f},{ "V", (int)Con_Float, 0.f,0.f,0.f,1.f } }
				}

				,
				{
					"PBR", hcMaterial
					,{ { "Diffuse", (int)Con_Float4 },{ "Normal", (int)Con_Float4 },{ "Roughness", (int)Con_Float4 },{ "Displacement", (int)Con_Float4 } }
				,{ { "", (int)Con_Float4 } }
				,{ { "view", (int)Con_Float2, 1.f,0.f,0.f,1.f, true } }
				}

			};

		return metaNodes;
	}
	
	const float PI = 3.14159f;
	float RadToDeg(float a) { return a * 180.f / PI; }
	float DegToRad(float a) { return a / 180.f * PI; }
	void EditNode()
	{
		size_t index = mSelectedNodeIndex;

		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		bool dirty = false;
		const MetaNode& currentMeta = metaNodes[mNodes[index].mType];
		if (!ImGui::CollapsingHeader(currentMeta.mName, 0, ImGuiTreeNodeFlags_DefaultOpen))
			return;

		const NodeGraphDelegate::Con * param = currentMeta.mParams;
		unsigned char *paramBuffer = (unsigned char*)mNodes[index].mParameters;
		for (int i = 0; i < MaxCon; i++, param++)
		{
			if (!param->mName)
				break;
			switch (param->mType)
			{
			case Con_Float:
				dirty |= ImGui::InputFloat(param->mName, (float*)paramBuffer);
				break;
			case Con_Float2:
				dirty |= ImGui::InputFloat2(param->mName, (float*)paramBuffer);
				break;
			case Con_Float3:
				dirty |= ImGui::InputFloat3(param->mName, (float*)paramBuffer);
				break;
			case Con_Float4:
				dirty |= ImGui::InputFloat4(param->mName, (float*)paramBuffer);
				break;
			case Con_Color4:
				dirty |= ImGui::ColorPicker4(param->mName, (float*)paramBuffer);
				break;
			case Con_Int:
				dirty |= ImGui::InputInt(param->mName, (int*)paramBuffer);
				break;
			case Con_Ramp:
				{
					ImVec2 points[8];
					
					for (int k = 0; k < 8; k++)
					{
						points[k] = ImVec2(((float*)paramBuffer)[k * 2], ((float*)paramBuffer)[k * 2 + 1]);
						if (k && points[k - 1].x > points[k].x)
							points[k] = ImVec2(1.f, 1.f);
					}

					if (ImGui::Curve("Ramp", ImVec2(250, 150), 8, points))
					{
						for (int k = 0; k < 8; k++)
						{
							((float*)paramBuffer)[k * 2] = points[k].x;
							((float*)paramBuffer)[k * 2 + 1] = points[k].y;
						}
						dirty = true;
					}
				}
				break;
			case Con_Angle:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				dirty |= ImGui::InputFloat(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				break;
			case Con_Angle2:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = RadToDeg(((float*)paramBuffer)[1]);
				dirty |= ImGui::InputFloat2(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = DegToRad(((float*)paramBuffer)[1]);
				break;
			case Con_Angle3:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = RadToDeg(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = RadToDeg(((float*)paramBuffer)[2]);
				dirty |= ImGui::InputFloat3(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = DegToRad(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = DegToRad(((float*)paramBuffer)[2]);
				break;
			case Con_Angle4:
				((float*)paramBuffer)[0] = RadToDeg(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = RadToDeg(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = RadToDeg(((float*)paramBuffer)[2]);
				((float*)paramBuffer)[3] = RadToDeg(((float*)paramBuffer)[3]);
				dirty |= ImGui::InputFloat4(param->mName, (float*)paramBuffer);
				((float*)paramBuffer)[0] = DegToRad(((float*)paramBuffer)[0]);
				((float*)paramBuffer)[1] = DegToRad(((float*)paramBuffer)[1]);
				((float*)paramBuffer)[2] = DegToRad(((float*)paramBuffer)[2]);
				((float*)paramBuffer)[3] = DegToRad(((float*)paramBuffer)[3]);
				break;
			case Con_Enum:
				dirty |= ImGui::Combo(param->mName, (int*)paramBuffer, param->mEnumList);
				break;
			}
			paramBuffer += ComputeParamMemSize(param->mType);
		}
		
		//ImGui::End();
		if (dirty)
			mEvaluation.SetEvaluationParameters(mNodes[index].mEvaluationTarget, mNodes[index].mParameters, mNodes[index].mParametersSize);
	}

	void InvalidateParameters()
	{
		for (auto& node : mNodes)
			mEvaluation.SetEvaluationParameters(node.mEvaluationTarget, node.mParameters, node.mParametersSize);
	}

	void SetMouseRatios(float rx, float ry, float dx, float dy)
	{
		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		size_t res = 0;
		const NodeGraphDelegate::Con * param = metaNodes[mNodes[mSelectedNodeIndex].mType].mParams;
		unsigned char *paramBuffer = (unsigned char*)mNodes[mSelectedNodeIndex].mParameters;
		for (int i = 0; i < MaxCon; i++, param++)
		{
			if (!param->mName)
				break;
			float *paramFlt = (float*)paramBuffer;
			if (param->mRangeMinX != 0.f || param->mRangeMaxX != 0.f)
			{
				if (param->mbRelative)
				{
					paramFlt[0] += ImLerp(param->mRangeMinX, param->mRangeMaxX, dx);
				}
				else
				{
					paramFlt[0] = ImLerp(param->mRangeMinX, param->mRangeMaxX, rx);
				}
				paramFlt[0] = fmodf(paramFlt[0], fabsf(param->mRangeMaxX - param->mRangeMinX)) + min(param->mRangeMinX, param->mRangeMaxX);
			}
			if (param->mRangeMinY != 0.f || param->mRangeMaxY != 0.f)
			{
				if (param->mbRelative)
				{
					paramFlt[1] += ImLerp(param->mRangeMinY, param->mRangeMaxY, dy);
				}
				else
				{
					paramFlt[1] = ImLerp(param->mRangeMinY, param->mRangeMaxY, ry);
				}

				paramFlt[1] = fmodf(paramFlt[1], fabsf(param->mRangeMaxY - param->mRangeMinY)) + min(param->mRangeMinY, param->mRangeMaxY);
			}
			paramBuffer += ComputeParamMemSize(param->mType);
		}
		mEvaluation.SetEvaluationParameters(mNodes[mSelectedNodeIndex].mEvaluationTarget, mNodes[mSelectedNodeIndex].mParameters, mNodes[mSelectedNodeIndex].mParametersSize);
	}

	size_t ComputeParamMemSize(size_t typeIndex)
	{
		int metaNodeCount;
		const MetaNode* metaNodes = GetMetaNodes(metaNodeCount);
		size_t res = 0;
		const NodeGraphDelegate::Con * param = metaNodes[typeIndex].mParams;
		for (int i = 0; i < MaxCon; i++, param++)
		{
			if (!param->mName)
				break;
			res += ComputeParamMemSize(param->mType);
		}
		return res;
	}
	size_t ComputeParamMemSize(int paramType)
	{
		size_t res = 0;
		switch (paramType)
		{
		case Con_Angle:
		case Con_Float:
			res += sizeof(float);
			break;
		case Con_Angle2:
		case Con_Float2:
			res += sizeof(float) * 2;
			break;
		case Con_Angle3:
		case Con_Float3:
			res += sizeof(float) * 3;
			break;
		case Con_Angle4:
		case Con_Color4:
		case Con_Float4:
			res += sizeof(float) * 4;
			break;
		case Con_Ramp:
			res += sizeof(float) * 2 * 8;
			break;
		case Con_Enum:
		case Con_Int:
			res += sizeof(int);
			break;
			
		}
		return res;
	}

	virtual void UpdateEvaluationList(const std::vector<size_t> nodeOrderList)
	{
		mEvaluation.SetEvaluationOrder(nodeOrderList);
	}

	virtual void Bake(size_t index)
	{
		mEvaluation.Bake("bakedTexture.png", index, 4096, 4096);
	}
};

