#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/Math/Vec4.h>
#include <PIMM/Math/MathUtility.h>
#include <cmath>
#include <cassert>

namespace pimm
{
	class Matrix4x4
	{
		public:
			//CONSTRUCTOR
			Matrix4x4() = default;

			//Identity Matrix
			static Matrix4x4 Identity() noexcept
			{
				Matrix4x4 identityMatrix{};

				identityMatrix.m_data[0][0] = 1;
				identityMatrix.m_data[1][1] = 1;
				identityMatrix.m_data[2][2] = 1;
				identityMatrix.m_data[3][3] = 1;

				return identityMatrix;

			}

			static Matrix4x4 Translate(const Vec3& translation) noexcept
			{
				auto matrix = Matrix4x4::Identity();

				matrix.m_data[3][0] = translation.x;
				matrix.m_data[3][1] = translation.y;
				matrix.m_data[3][2] = translation.z;

				return matrix;
			}

			static Matrix4x4 Scale(const Vec3& scale) noexcept
			{
				Matrix4x4 matrix{};

				matrix.m_data[0][0] = scale.x;
				matrix.m_data[1][1] = scale.y;
				matrix.m_data[2][2] = scale.z;
				matrix.m_data[3][3] = 1;

				return matrix;
			}

			static Matrix4x4 RotateAlongX(f32 xRotation) noexcept
			{
				Matrix4x4 matrix{};

				xRotation = MathUtility::ToRadians(xRotation);

				matrix.m_data[0][0] = 1;
				matrix.m_data[1][1] = std::cos(xRotation);
				matrix.m_data[1][2] = std::sin(xRotation);
				matrix.m_data[2][1] = -std::sin(xRotation);
				matrix.m_data[2][2] = std::cos(xRotation);
				matrix.m_data[3][3] = 1;

				return matrix;
			}

			static Matrix4x4 RotateAlongY(f32 yRotation) noexcept
			{
				Matrix4x4 matrix{};

				yRotation = MathUtility::ToRadians(yRotation);

				matrix.m_data[0][0] = std::cos(yRotation);
				matrix.m_data[0][2] = -std::sin(yRotation);
				matrix.m_data[1][1] = 1;
				matrix.m_data[2][0] = std::sin(yRotation);
				matrix.m_data[2][2] = std::cos(yRotation);
				matrix.m_data[3][3] = 1;

				return matrix;
			}

			static Matrix4x4 RotateAlongZ(f32 zRotation) noexcept
			{
				Matrix4x4 matrix{};

				zRotation = MathUtility::ToRadians(zRotation);

				matrix.m_data[0][0] = std::cos(zRotation);
				matrix.m_data[0][1] = std::sin(zRotation);
				matrix.m_data[1][0] = -std::sin(zRotation);
				matrix.m_data[1][1] = std::cos(zRotation);
				matrix.m_data[2][2] = 1;
				matrix.m_data[3][3] = 1;

				return matrix;
			}

			Matrix4x4 operator *(const Matrix4x4& rightHandSide) const noexcept
			{
				Matrix4x4 matrix{};

				for (auto row = 0u; row < 4u; row++)
				{
					for (auto i = 0u; i < 4u; i++)
					{
						auto tempMatrix = m_data[row][i];
						for (auto col = 0u; col < 4u; col++)
						{
							matrix.m_data[row][col] += tempMatrix * rightHandSide.m_data[i][col];
						}
					}
				}

				return matrix;
			}

			static Matrix4x4 OrthoLH(f32 width, f32 height, f32 zNear, f32 zFar) noexcept
			{
				assert(width != 0.0f && "OrthoLH: width must not be zero");
				assert(height != 0.0f && "OrthoLH: height must not be zero");
				assert(zFar != zNear && "OrthoLH: zNear and zFar cannot be equal");

				Matrix4x4 matrix{};

				matrix.m_data[0][0] = 2.0f / width;
				matrix.m_data[1][1] = 2.0f / height;
				matrix.m_data[2][2] = 1.0f / (zFar - zNear);
				matrix.m_data[3][2] = -(zNear / (zFar - zNear));
				matrix.m_data[3][3] = 1;

				return matrix;
			}

			static Matrix4x4 PerspectiveFOVLH(f32 FOV, f32 aspectRatio, f32 zNear, f32 zFar) noexcept
			{
				assert(FOV > 0.001f && "PerspectiveFOVLH: FOV must be greater than 0 radians");
				assert(FOV < MathUtility::PI - 0.001f && "PerspectiveFOVLH: FOV must be less than PI radians.");
				assert(aspectRatio > 0.0f && "PerspectiveFOVLH: Aspect Ratio must be greater than 0");
				assert(zFar != zNear && "PerspectiveFOVLH: zNear and zFar cannot be equal.");

				Matrix4x4 matrix{};

				f32 yScale = 1.0f / tan(FOV / 2.0f);
				f32 xScale = yScale / aspectRatio;

				matrix.m_data[0][0] = xScale;
				matrix.m_data[1][1] = yScale;
				matrix.m_data[2][2] = zFar / (zFar - zNear);
				matrix.m_data[2][3] = 1.0f;
				matrix.m_data[3][2] = (-zNear * zFar) / (zFar - zNear);
				matrix.m_data[3][3] = 0.0f;

				return matrix;
			}

			static Matrix4x4 Inverse(const Matrix4x4& rightHandSide) noexcept
			{
				Matrix4x4 output{};

				Vec4 vec[3]{};

				auto determinant = Matrix4x4::Determinant(rightHandSide);
				if (!determinant) return{};

				for (auto i = 0; i < 4; i++)
				{
					for (auto j = 0; j < 4; j++)
					{
						if (j != i)
						{
							auto a = j;
							if (j > i) a = a - 1;
							vec[a].x = (rightHandSide.m_data[j][0]);
							vec[a].y = (rightHandSide.m_data[j][1]);
							vec[a].z = (rightHandSide.m_data[j][2]);
							vec[a].w = (rightHandSide.m_data[j][3]);
						}
					}
					auto v = Vec4::Cross(vec[0], vec[1], vec[2]);

					output.m_data[0][i] = (f32)std::pow(-1.0f, i) * v.x / determinant;
					output.m_data[1][i] = (f32)std::pow(-1.0f, i) * v.y / determinant;
					output.m_data[2][i] = (f32)std::pow(-1.0f, i) * v.z / determinant;
					output.m_data[3][i] = (f32)std::pow(-1.0f, i) * v.w / determinant;
				}

				return output;
			}

			static f32 Determinant(const Matrix4x4& rightHandSide) noexcept
			{
				auto vector1 = Vec4
				(
					rightHandSide.m_data[0][0],
					rightHandSide.m_data[1][0],
					rightHandSide.m_data[2][0],
					rightHandSide.m_data[3][0]
				);

				auto vector2 = Vec4
				(
					rightHandSide.m_data[0][1],
					rightHandSide.m_data[1][1],
					rightHandSide.m_data[2][1],
					rightHandSide.m_data[3][1]
				);

				auto vector3 = Vec4
				(
					rightHandSide.m_data[0][2],
					rightHandSide.m_data[1][2],
					rightHandSide.m_data[2][2],
					rightHandSide.m_data[3][2]
				);

				auto minor = Vec4::Cross(vector1, vector2, vector3);

				auto det =
					-(
						rightHandSide.m_data[0][3] * minor.x +
						rightHandSide.m_data[1][3] * minor.y +
						rightHandSide.m_data[2][3] * minor.z +
						rightHandSide.m_data[3][3] * minor.w
						);

				return det;
			}

			Vec4 Row(ui32 index) const
			{
				assert(index < 4 && "Matrix row index out of range");
				return
				{
					m_data[index][0],
					m_data[index][1],
					m_data[index][2],
					m_data[index][3]
				};
			}

			Vec4 Column(ui32 index) const
			{
				assert(index < 4 && "Matrix column index out of range");
				return
				{
					m_data[0][index],
					m_data[1][index],
					m_data[2][index],
					m_data[3][index]
				};
			}

		private:
			f32 m_data[4][4]{};
	};
}