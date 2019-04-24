﻿/*#######################################################################################
	Copyright (c) 2017-2019 Kasugaccho
	https://github.com/Kasugaccho/DungeonTemplateLibrary
	wanotaitei@gmail.com

	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#######################################################################################*/
#ifndef INCLUDED_DUNGEON_TEMPLATE_LIBRARY_SHAPE_BORDER
#define INCLUDED_DUNGEON_TEMPLATE_LIBRARY_SHAPE_BORDER

/*#######################################################################################
	日本語リファレンス (Reference-JP)
	https://github.com/Kasugaccho/DungeonTemplateLibrary/wiki/dtl::shape::Border-(%E5%BD%A2%E7%8A%B6%E3%82%AF%E3%83%A9%E3%82%B9)/
#######################################################################################*/

/* Character Code : UTF-8 (BOM) */
/* Bug Check : already checked */
/* Android NDK Compile (Clang 5.0) : already checked */

#include <cstddef>
#include <cstdint>
#include <utility>
#include <Base/Struct.hpp>

namespace dtl {
	inline namespace shape {

		//マップの外枠を指定した数値で埋める
		template<typename Matrix_Int_>
		class Border {
		private:


			///// エイリアス /////

			using Index_Size = std::size_t;
			using PairSize = std::pair<Index_Size, Index_Size>;


			///// メンバ変数 /////

			Index_Size point_x{};
			Index_Size point_y{};
			Index_Size width{};
			Index_Size height{};
			Matrix_Int_ draw_value{};


			///// 代入処理 /////

			template<typename Matrix_>
			constexpr inline void substitutionSTL(Matrix_&& matrix_, const Index_Size point_x_, const Index_Size point_y_) const noexcept {
				matrix_[point_y_][point_x_] = this->draw_value;
			}
			template<typename Matrix_>
			constexpr inline void substitutionArray(Matrix_&& matrix_, const Index_Size point_x_, const Index_Size point_y_, const Index_Size max_x_) const noexcept {
				matrix_[point_y_ * max_x_ + point_x_] = this->draw_value;
			}
			template<typename Matrix_>
			constexpr inline void substitutionLayer(Matrix_&& matrix_, const Index_Size layer_, const Index_Size point_x_, const Index_Size point_y_) const noexcept {
				matrix_[point_y_][point_x_][layer_] = this->draw_value;
			}

			template<typename Matrix_, typename Function_>
			constexpr inline void substitutionSTL(Matrix_&& matrix_, const Index_Size point_x_, const Index_Size point_y_, Function_&& function_) const noexcept {
				if (function_(matrix_[point_y_][point_x_])) matrix_[point_y_][point_x_] = this->draw_value;
			}
			template<typename Matrix_, typename Function_>
			constexpr inline void substitutionArray(Matrix_&& matrix_, const Index_Size point_x_, const Index_Size point_y_, const Index_Size max_x_, Function_&& function_) const noexcept {
				if (function_(matrix_[point_y_ * max_x_ + point_x_])) matrix_[point_y_ * max_x_ + point_x_] = this->draw_value;
			}
			template<typename Matrix_, typename Function_>
			constexpr inline void substitutionLayer(Matrix_ && matrix_, const Index_Size layer_, const Index_Size point_x_, const Index_Size point_y_, Function_ && function_) const noexcept {
				if (function_(matrix_[point_y_][point_x_][layer_])) matrix_[point_y_][point_x_][layer_] = this->draw_value;
			}


			///// 基本処理 /////

			//STL
			template<typename Matrix_, typename ...Args_>
			constexpr bool drawSTL(Matrix_ && matrix_, const Index_Size point_y_, Args_ && ... args_) const noexcept {
				if (point_y_ == 0) return true;
				for (Index_Size col{ this->point_x }; col < matrix_[this->point_y].size(); ++col)
					this->substitutionSTL(matrix_, col, this->point_y, args_...);
				for (Index_Size col{ this->point_x }; col < matrix_[point_y_ - 1].size(); ++col)
					this->substitutionSTL(matrix_, col, point_y_ - 1, args_...);
				for (Index_Size row{ this->point_y }; row < point_y_; ++row) {
					if (matrix_[row].size() == 0) continue;
					this->substitutionSTL(matrix_, this->point_x, row, args_...);
					this->substitutionSTL(matrix_, matrix_[row].size() - 1, row, std::forward<Args_>(args_)...);
				}
				return true;
			}
			template<typename Matrix_, typename ...Args_>
			constexpr bool drawWidthSTL(Matrix_ && matrix_, const Index_Size point_x_, const Index_Size point_y_, Args_ && ... args_) const noexcept {
				if (point_y_ == 0) return true;
				for (Index_Size col{ this->point_x }; col < point_x_ && col < matrix_[this->point_y].size(); ++col)
					this->substitutionSTL(matrix_, col, this->point_y, args_...);
				for (Index_Size col{ this->point_x }; col < point_x_ && col < matrix_[point_y_ - 1].size(); ++col)
					this->substitutionSTL(matrix_, col, point_y_ - 1, args_...);
				if (point_x_ == 0) return true;
				for (Index_Size row{ this->point_y }; row < point_y_; ++row) {
					if (matrix_[row].size() == 0) continue;
					this->substitutionSTL(matrix_, this->point_x, row, args_...);
					if (matrix_[row].size() <= point_x_) this->substitutionSTL(matrix_, matrix_[row].size() - 1, row, args_...);
					else this->substitutionSTL(matrix_, point_x_ - 1, row, std::forward<Args_>(args_)...);
				}
				return true;
			}

			//LayerSTL
			template<typename Matrix_, typename ...Args_>
			constexpr bool drawLayerSTL(Matrix_ && matrix_, const Index_Size layer_, const Index_Size point_y_, Args_ && ... args_) const noexcept {
				if (point_y_ == 0) return true;
				for (Index_Size col{ this->point_x }; col < matrix_[this->point_y].size(); ++col)
					this->substitutionLayer(matrix_, layer_, col, this->point_y, args_...);
				for (Index_Size col{ this->point_x }; col < matrix_[point_y_ - 1].size(); ++col)
					this->substitutionLayer(matrix_, layer_, col, point_y_ - 1, args_...);
				for (Index_Size row{ this->point_y }; row < point_y_; ++row) {
					if (matrix_[row].size() == 0) continue;
					this->substitutionLayer(matrix_, layer_, this->point_x, row, args_...);
					this->substitutionLayer(matrix_, layer_, matrix_[row].size() - 1, row, std::forward<Args_>(args_)...);
				}
				return true;
			}
			template<typename Matrix_, typename ...Args_>
			constexpr bool drawLayerWidthSTL(Matrix_ && matrix_, const Index_Size layer_, const Index_Size point_x_, const Index_Size point_y_, Args_ && ... args_) const noexcept {
				if (point_y_ == 0) return true;
				for (Index_Size col{ this->point_x }; col < point_x_ && col < matrix_[this->point_y].size(); ++col)
					this->substitutionLayer(matrix_, layer_, col, this->point_y, args_...);
				for (Index_Size col{ this->point_x }; col < point_x_ && col < matrix_[point_y_ - 1].size(); ++col)
					this->substitutionLayer(matrix_, layer_, col, point_y_ - 1, args_...);
				if (point_x_ == 0) return true;
				for (Index_Size row{ this->point_y }; row < point_y_; ++row) {
					if (matrix_[row].size() == 0) continue;
					this->substitutionLayer(matrix_, layer_, this->point_x, row, args_...);
					if (matrix_[row].size() <= point_x_) this->substitutionLayer(matrix_, layer_, matrix_[row].size() - 1, row, args_...);
					else this->substitutionLayer(matrix_, layer_, point_x_ - 1, row, std::forward<Args_>(args_)...);
				}
				return true;
			}

			//Normal
			template<typename Matrix_, typename ...Args_>
			constexpr bool drawNormal(Matrix_ && matrix_, const Index_Size point_x_, const Index_Size point_y_, Args_ && ... args_) const noexcept {
				if (point_x_ == 0 || point_y_ == 0) return true;
				for (Index_Size col{ this->point_x }; col < point_x_; ++col) {
					this->substitutionSTL(matrix_, col, this->point_y, args_...);
					this->substitutionSTL(matrix_, col, point_y_ - 1, args_...);
				}
				for (Index_Size row{ this->point_y }; row < point_y_; ++row) {
					this->substitutionSTL(matrix_, this->point_x, row, args_...);
					this->substitutionSTL(matrix_, point_x_ - 1, row, std::forward<Args_>(args_)...);
				}
				return true;
			}

			//LayerNormal
			template<typename Matrix_, typename ...Args_>
			constexpr bool drawLayerNormal(Matrix_ && matrix_, const Index_Size layer_, const Index_Size point_x_, const Index_Size point_y_, Args_ && ... args_) const noexcept {
				if (point_x_ == 0 || point_y_ == 0) return true;
				for (Index_Size col{ this->point_x }; col < point_x_; ++col) {
					this->substitutionLayer(matrix_, layer_, col, this->point_y, args_...);
					this->substitutionLayer(matrix_, layer_, col, point_y_ - 1, args_...);
				}
				for (Index_Size row{ this->point_y }; row < point_y_; ++row) {
					this->substitutionLayer(matrix_, layer_, this->point_x, row, args_...);
					this->substitutionLayer(matrix_, layer_, point_x_ - 1, row, std::forward<Args_>(args_)...);
				}
				return true;
			}

			//Array
			template<typename Matrix_, typename ...Args_>
			constexpr bool drawArray(Matrix_ && matrix_, const Index_Size point_x_, const Index_Size point_y_, const Index_Size max_x_, Args_ && ... args_) const noexcept {
				if (point_x_ == 0 || point_y_ == 0) return true;
				for (Index_Size col{ this->point_x }; col < point_x_; ++col) {
					this->substitutionArray(matrix_, col, this->point_y, max_x_, args_...);
					this->substitutionArray(matrix_, col, point_y_ - 1, max_x_, args_...);
				}
				for (Index_Size row{ this->point_y }; row < point_y_; ++row) {
					this->substitutionArray(matrix_, this->point_x, row, max_x_, args_...);
					this->substitutionArray(matrix_, point_x_ - 1, row, max_x_, std::forward<Args_>(args_)...);
				}
				return true;
			}

		public:


			///// 情報取得 /////

#ifdef __cplusplus
#if (__cplusplus >= 201703L)
			[[nodiscard]]
#endif
#endif
			constexpr Index_Size getPointX() const noexcept {
				return this->point_x;
			}
#ifdef __cplusplus
#if (__cplusplus >= 201703L)
			[[nodiscard]]
#endif
#endif
			constexpr Index_Size getPointY() const noexcept {
				return this->point_y;
			}
#ifdef __cplusplus
#if (__cplusplus >= 201703L)
			[[nodiscard]]
#endif
#endif
			constexpr Index_Size getWidth() const noexcept {
				return this->width;
			}
#ifdef __cplusplus
#if (__cplusplus >= 201703L)
			[[nodiscard]]
#endif
#endif
			constexpr Index_Size getHeight() const noexcept {
				return this->height;
			}
#ifdef __cplusplus
#if (__cplusplus >= 201703L)
			[[nodiscard]]
#endif
#endif
			constexpr Matrix_Int_ getValue() const noexcept {
				return this->draw_value;
			}


			///// 生成呼び出し /////

			//STL
			template<typename Matrix_>
			constexpr bool draw(Matrix_ && matrix_) const noexcept {
				return (this->width == 0) ? this->drawSTL(std::forward<Matrix_>(matrix_), (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height) : this->drawWidthSTL(matrix_, this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height);
			}
			template<typename Matrix_, typename Function_>
			constexpr bool drawOperator(Matrix_ && matrix_, Function_ && function_) const noexcept {
				return (this->width == 0) ? this->drawSTL(std::forward<Matrix_>(matrix_), (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height, function_) : this->drawWidthSTL(matrix_, this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height, function_);
			}

			//LayerSTL
			template<typename Matrix_>
			constexpr bool draw(Matrix_ && matrix_, const Index_Size layer_) const noexcept {
				return (this->width == 0) ? this->drawLayerSTL(std::forward<Matrix_>(matrix_), layer_, (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height) : this->drawLayerWidthSTL(matrix_, layer_, this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height);
			}
			template<typename Matrix_, typename Function_>
			constexpr bool drawOperator(Matrix_ && matrix_, const Index_Size layer_, Function_ && function_) const noexcept {
				return (this->width == 0) ? this->drawLayerSTL(std::forward<Matrix_>(matrix_), layer_, (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height, function_) : this->drawLayerWidthSTL(matrix_, layer_, this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= matrix_.size()) ? matrix_.size() : this->point_y + this->height, function_);
			}

			//Normal
			template<typename Matrix_>
			constexpr bool draw(Matrix_ && matrix_, const Index_Size max_x_, const Index_Size max_y_) const noexcept {
				return this->drawNormal(std::forward<Matrix_>(matrix_), (this->width == 0 || this->point_x + this->width >= max_x_) ? max_x_ : this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= max_y_) ? max_y_ : this->point_y + this->height);
			}
			template<typename Matrix_, typename Function_>
			constexpr bool drawOperator(Matrix_ && matrix_, const Index_Size max_x_, const Index_Size max_y_, Function_ && function_) const noexcept {
				return this->drawNormal(std::forward<Matrix_>(matrix_), (this->width == 0 || this->point_x + this->width >= max_x_) ? max_x_ : this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= max_y_) ? max_y_ : this->point_y + this->height, function_);
			}

			//LayerNormal
			template<typename Matrix_>
			constexpr bool draw(Matrix_ && matrix_, const Index_Size layer_, const Index_Size max_x_, const Index_Size max_y_) const noexcept {
				return this->drawLayerNormal(std::forward<Matrix_>(matrix_), layer_, (this->width == 0 || this->point_x + this->width >= max_x_) ? max_x_ : this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= max_y_) ? max_y_ : this->point_y + this->height);
			}
			template<typename Matrix_, typename Function_>
			constexpr bool drawOperator(Matrix_ && matrix_, const Index_Size layer_, const Index_Size max_x_, const Index_Size max_y_, Function_ && function_) const noexcept {
				return this->drawLayerNormal(std::forward<Matrix_>(matrix_), layer_, (this->width == 0 || this->point_x + this->width >= max_x_) ? max_x_ : this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= max_y_) ? max_y_ : this->point_y + this->height, function_);
			}

			//Array
			template<typename Matrix_>
			constexpr bool drawArray(Matrix_ && matrix_, const Index_Size max_x_, const Index_Size max_y_) const noexcept {
				return this->drawArray(std::forward<Matrix_>(matrix_), (this->width == 0 || this->point_x + this->width >= max_x_) ? max_x_ : this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= max_y_) ? max_y_ : this->point_y + this->height, max_x_);
			}
			template<typename Matrix_, typename Function_>
			constexpr bool drawOperatorArray(Matrix_ && matrix_, const Index_Size max_x_, const Index_Size max_y_, Function_ && function_) const noexcept {
				return this->drawArray(std::forward<Matrix_>(matrix_), (this->width == 0 || this->point_x + this->width >= max_x_) ? max_x_ : this->point_x + this->width, (this->height == 0 || this->point_y + this->height >= max_y_) ? max_y_ : this->point_y + this->height, max_x_, function_);
			}


			///// 生成呼び出しファンクタ /////

			template<typename Matrix_, typename ...Args_>
			constexpr auto operator()(Matrix_&& matrix_, Args_&& ... args_) const noexcept {
				return this->draw(std::forward<Matrix_>(matrix_), std::forward<Args_>(args_)...);
			}


			///// ダンジョン行列生成 /////

			template<typename Matrix_, typename ...Args_>
			constexpr auto create(Matrix_ && matrix_, Args_ && ... args_) const noexcept {
				this->draw(matrix_, std::forward<Args_>(args_)...);
				return std::forward<Matrix_>(matrix_);
			}
			template<typename Matrix_, typename ...Args_>
			constexpr auto createArray(Matrix_ && matrix_, Args_ && ... args_) const noexcept {
				this->drawArray(matrix_, std::forward<Args_>(args_)...);
				return std::forward<Matrix_>(matrix_);
			}
			template<typename Matrix_, typename ...Args_>
			constexpr auto createOperator(Matrix_ && matrix_, Args_ && ... args_) const noexcept {
				this->drawOperator(matrix_, std::forward<Args_>(args_)...);
				return std::forward<Matrix_>(matrix_);
			}
			template<typename Matrix_, typename ...Args_>
			constexpr auto createOperatorArray(Matrix_ && matrix_, Args_ && ... args_) const noexcept {
				this->drawOperatorArray(matrix_, std::forward<Args_>(args_)...);
				return std::forward<Matrix_>(matrix_);
			}


			///// 消去 /////

			//始点座標Xを初期値に戻す
			constexpr auto clearPointX() noexcept {
				this->point_x = 0;
				return *this;
			}
			//始点座標Yを初期値に戻す
			constexpr auto clearPointY() noexcept {
				this->point_y = 0;
				return *this;
			}
			//範囲の大きさ(X軸方向)を初期値に戻す
			constexpr auto clearWidth() noexcept {
				this->width = 0;
				return *this;
			}
			//範囲の大きさ(Y軸方向)を初期値に戻す
			constexpr auto clearHeight() noexcept {
				this->height = 0;
				return *this;
			}
			//塗り値を初期値に戻す
			constexpr auto clearValue() noexcept {
				const Matrix_Int_ new_draw_value{};
				this->draw_value = new_draw_value;
				return *this;
			}
			//始点座標(X,Y)を初期値に戻す
			constexpr auto clearPoint() noexcept {
				this->clearPointX();
				this->clearPointY();
				return *this;
			}
			//描画範囲を初期値に戻す
			constexpr auto clearRange() noexcept {
				this->clearPointX();
				this->clearPointY();
				this->clearWidth();
				this->clearHeight();
				return *this;
			}
			//全ての値を初期値に戻す
			constexpr auto clear() noexcept {
				this->clearRange();
				this->clearValue();
				return *this;
			}


			///// 代入 /////

			constexpr auto setPointX(const Index_Size point_x_) noexcept {
				this->point_x = point_x_;
				return *this;
			}
			constexpr auto setPointY(const Index_Size point_y_) noexcept {
				this->point_y = point_y_;
				return *this;
			}
			constexpr auto setWidth(const Index_Size width_) noexcept {
				this->width = width_;
				return *this;
			}
			constexpr auto setHeight(const Index_Size height_) noexcept {
				this->height = height_;
				return *this;
			}
			constexpr auto setValue(const Matrix_Int_ & draw_value_) noexcept {
				this->draw_value = draw_value_;
				return *this;
			}
			constexpr auto setPoint(const Index_Size point_) noexcept {
				this->point_x = point_;
				this->point_y = point_;
				return *this;
			}
			constexpr auto setPoint(const Index_Size point_x_, const Index_Size point_y_) noexcept {
				this->point_x = point_x_;
				this->point_y = point_y_;
				return *this;
			}
			constexpr auto setRange(const Index_Size point_x_, const Index_Size point_y_, const Index_Size length_) noexcept {
				this->point_x = point_x_;
				this->point_y = point_y_;
				this->width = length_;
				this->height = length_;
				return *this;
			}
			constexpr auto setRange(const Index_Size point_x_, const Index_Size point_y_, const Index_Size width_, const Index_Size height_) noexcept {
				this->point_x = point_x_;
				this->point_y = point_y_;
				this->width = width_;
				this->height = height_;
				return *this;
			}
			constexpr auto setRange(const dtl::base::MatrixRange & matrix_range_) noexcept {
				this->point_x = matrix_range_.x;
				this->point_y = matrix_range_.y;
				this->width = matrix_range_.w;
				this->height = matrix_range_.h;
				return *this;
			}


			///// コンストラクタ /////

			constexpr Border() noexcept = default;
			constexpr explicit Border(const Matrix_Int_ & draw_value_) noexcept
				:draw_value(draw_value_) {}
			constexpr explicit Border(const dtl::base::MatrixRange & matrix_range_) noexcept
				:point_x(matrix_range_.x), point_y(matrix_range_.y),
				width(matrix_range_.w), height(matrix_range_.h) {}
			constexpr explicit Border(const dtl::base::MatrixRange & matrix_range_, const Matrix_Int_ & draw_value_) noexcept
				:point_x(matrix_range_.x), point_y(matrix_range_.y),
				width(matrix_range_.w), height(matrix_range_.h),
				draw_value(draw_value_) {}
			constexpr explicit Border(const Index_Size point_x_, const Index_Size point_y_, const Index_Size width_, const Index_Size height_) noexcept
				:point_x(point_x_), point_y(point_y_),
				width(width_), height(height_) {}
			constexpr explicit Border(const Index_Size point_x_, const Index_Size point_y_, const Index_Size width_, const Index_Size height_, const Matrix_Int_ & draw_value_) noexcept
				:point_x(point_x_), point_y(point_y_),
				width(width_), height(height_),
				draw_value(draw_value_) {}
		};
	}
}

#endif //Included Dungeon Template Library