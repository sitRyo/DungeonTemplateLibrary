﻿#ifndef INCLUDED_DUNGEON_TEMPLATE_LIBRARY_BOARD_GAME
#define INCLUDED_DUNGEON_TEMPLATE_LIBRARY_BOARD_GAME
//:::::----------::::::::::----------::::://
//     Dungeon Template Library     //
//          Made by Gaccho.          //
// This code is licensed under CC0.  //
//       wanotaitei@gmail.com       //
//:::::----------::::::::::----------::::://

/* [2019/03/08] Android NDK Compile (Clang 5.0) : already checked */

#include <cstddef>
#include <cstdint>
#include <array>
#include <limits>
#include <memory>

//Dungeon Template Library Namespace
namespace dtl {
	namespace ai {

		namespace reversi {

			//指定した場所に駒を置く
			template<typename Matrix_Int_, typename Matrix_>
			std::size_t putPiece(Matrix_& matrix_, const std::size_t  col_, const std::size_t row_, const Matrix_Int_ turn_, const bool is_put_ = true) noexcept {
				if (matrix_.size() == 0) return 0;
				std::size_t piece_turn_num{};
				if (matrix_[row_][col_] > 0) return 0;

				std::unique_ptr<std::int_fast32_t[]> stl_tmp_x{ std::make_unique<std::int_fast32_t[]>(matrix_[0].size()) };
				std::unique_ptr<std::int_fast32_t[]> stl_tmp_y{ std::make_unique<std::int_fast32_t[]>(matrix_.size()) };

				for (std::int_fast32_t y{ -1 }; y <= 1; ++y)
					for (std::int_fast32_t x{ -1 }; x <= 1; ++x) {
						for (std::size_t i{}; i < matrix_[0].size(); ++i) stl_tmp_x[i] = 0;
						for (std::size_t i{}; i < matrix_.size(); ++i) stl_tmp_y[i] = 0;
						for (std::size_t turn_tmp_id{};; ++turn_tmp_id) {
							std::int_fast32_t turn_x{ static_cast<std::int_fast32_t>(col_) + x * (static_cast<std::int_fast32_t>(turn_tmp_id) + 1) };
							std::int_fast32_t turn_y{ static_cast<std::int_fast32_t>(row_) + y * (static_cast<std::int_fast32_t>(turn_tmp_id) + 1) };
							if (turn_x < 0 || turn_x >= matrix_[0].size() || turn_y < 0 || turn_y >= matrix_.size() || matrix_[turn_y][turn_x] == 0) break;
							if (matrix_[turn_y][turn_x] == turn_) {
								if (is_put_)
									for (std::size_t i{}; i < turn_tmp_id; ++i)
										matrix_[static_cast<std::size_t>(stl_tmp_y[i])][static_cast<std::size_t>(stl_tmp_x[i])] = turn_;
								piece_turn_num += turn_tmp_id;
								break;
							}
							stl_tmp_x[turn_tmp_id] = turn_x; stl_tmp_y[turn_tmp_id] = turn_y;
						}
					}
				if (piece_turn_num > 0 && is_put_) matrix_[row_][col_] = turn_;
				return piece_turn_num;
			}

			//パスの有無
			template<typename Matrix_Int_, typename Matrix_>
			constexpr bool isPass(Matrix_& matrix_, const Matrix_Int_ turn_) noexcept {
				for (std::size_t row{}; row < matrix_.size(); ++row)
					for (std::size_t col{}; col < matrix_[row].size(); ++col)
						if (dtl::ai::reversi::putPiece(matrix_, col, row, turn_, false)) return false;
				return true;
			}
			//最初に見つけた置ける場所を選ぶ
			class Simple {
			public:
				template<typename Matrix_Int_, typename Matrix_>
				constexpr bool operator()(Matrix_& matrix_, const Matrix_Int_ turn_) const noexcept {
					for (std::size_t row{}; row < matrix_.size(); ++row)
						for (std::size_t col{}; col < matrix_[row].size(); ++col)
							if (dtl::ai::reversi::putPiece(matrix_, col, row, turn_, true)) return true;
					return true;
				}
			};
			//最も多くの駒が取れる場所を選ぶ
			class Greed {
			public:
				template<typename Matrix_Int_, typename Matrix_>
				constexpr bool operator()(Matrix_& matrix_, const Matrix_Int_ turn_) const noexcept {

					using dtl::random::mersenne_twister_32bit;
					using dtl::ai::reversi::putPiece;

					std::size_t piece_turn_max{};
					std::size_t put_piece_x{}, put_piece_y{};
					for (std::size_t row{}; row < matrix_.size(); ++row)
						for (std::size_t col{}; col < matrix_[row].size(); ++col) {
							const auto& num{ putPiece(matrix_, col,row, turn_, false) };
							if (piece_turn_max < num || (piece_turn_max == num && mersenne_twister_32bit.probability())) {
								piece_turn_max = num;
								put_piece_x = col;
								put_piece_y = row;
							}
						}
					putPiece(matrix_, put_piece_x, put_piece_y, turn_, true);
					return true;
				}
			};
			//最も少なく駒が取れる場所を選ぶ
			class Unselfishness {
			public:
				template<typename Matrix_Int_, typename Matrix_>
				constexpr bool operator()(Matrix_& matrix_, const Matrix_Int_ turn_) const noexcept {
					std::size_t piece_turn_min{ (std::numeric_limits<std::size_t>::max)() };
					std::size_t put_piece_x{}, put_piece_y{};
					for (std::size_t row{}; row < matrix_.size(); ++row)
						for (std::size_t col{}; col < matrix_[row].size(); ++col) {
							const auto& num{ dtl::ai::reversi::putPiece(matrix_, col,row, turn_, false) };
							if (num > 0 && (piece_turn_min > num || (piece_turn_min == num && dtl::random::mersenne_twister_32bit.probability()))) {
								piece_turn_min = num;
								put_piece_x = col;
								put_piece_y = row;
							}
						}
					dtl::ai::reversi::putPiece(matrix_, put_piece_x, put_piece_y, turn_, true);
					return true;
				}
			};

			//優先順位の高い場所を選ぶ
			class Priority {
			public:
				//優先順位
				constexpr std::uint_fast8_t checkPriority(std::size_t x_, std::size_t y_, const std::size_t x_max_, const std::size_t y_max_) const noexcept {
					if (x_ == x_max_) x_ = 0;
					else if (x_ == x_max_ - 1) x_ = 1;
					else if (x_ == x_max_ - 2) x_ = 2;
					else if (x_ >= 4) x_ = 3;

					if (y_ == y_max_) y_ = 0;
					else if (y_ == y_max_ - 1) y_ = 1;
					else if (y_ == y_max_ - 2) y_ = 2;
					else if (y_ >= 4) y_ = 3;

					constexpr std::array<uint_fast8_t, 16> check_point{ { 0,6,2,1,6,6,5,4,2,6,2,3,1,4,3,3 } };
					return check_point[y_ * 4 + x_];
				}
				template<typename Matrix_Int_, typename Matrix_>
				constexpr bool operator()(Matrix_& matrix_, const Matrix_Int_ turn_) const noexcept {

					using dtl::random::mersenne_twister_32bit;

					std::size_t piece_turn_max{};
					std::size_t put_piece_x{}, put_piece_y{};
					for (std::uint_fast8_t piece_priority{}; piece_priority < 7 && piece_turn_max == 0; ++piece_priority)
						for (std::size_t row{}; row < matrix_.size(); ++row)
							for (std::size_t col{}; col < matrix_[row].size(); ++col) {
								if (this->checkPriority(col, row, matrix_[row].size() - 1, matrix_.size() - 1) != piece_priority) continue;
								const auto& num{ dtl::ai::reversi::putPiece(matrix_, col, row, turn_, false) };
								if (piece_turn_max < num || (piece_turn_max == num && mersenne_twister_32bit.probability())) {
									piece_turn_max = num;
									put_piece_x = col;
									put_piece_y = row;
								}
							}
					dtl::ai::reversi::putPiece(matrix_, put_piece_x, put_piece_y, turn_, true);
					return true;
				}
			};
			//AI同士をまとめる関数
			template<typename Matrix_Int_, typename Matrix_,typename Black_,typename White_>
			constexpr bool reversiAI(Matrix_& matrix_, const Matrix_Int_ turn_, Black_&& black_ai_, White_&& white_ai_, const bool is_black_ai_) noexcept {
				return (is_black_ai_) ? black_ai_(matrix_, turn_) : white_ai_(matrix_, turn_);
			}

			template<typename Matrix_Int_, typename Matrix_>
			constexpr std::int_fast32_t checkResult(Matrix_& matrix_) noexcept {
				std::array<std::int_fast32_t, 2> piece_num{ {} };
				std::int_fast32_t result{};
				for (std::size_t row{}; row < matrix_.size(); ++row)
					for (std::size_t col{}; col < matrix_[row].size(); ++col)
						if (matrix_[row][col] > 0) ++piece_num[matrix_[row][col] - 1];
				if (dtl::ai::reversi::isPass(matrix_, (Matrix_Int_)1) && dtl::ai::reversi::isPass(matrix_, (Matrix_Int_)2)) {
					if (piece_num[0] > piece_num[1]) result = 1;
					else if (piece_num[0] < piece_num[1]) result = 2;
					else result = 3;
				}
				return result;
			}

		}

	}

	//地形生成
	namespace generator {
		namespace boardGame {

			namespace data {

				enum :std::size_t {
					reversi_empty,
					reversi_white,
					reversi_black
				};

			}

			namespace stl {

				//リバーシを初期化する
				template<typename Matrix_Int_, typename Matrix_>
				constexpr void createReversi(Matrix_& matrix_, const Matrix_Int_ black_ = 2, const Matrix_Int_ white_ = 1) noexcept {
					if (matrix_.size() < 2 || matrix_[0].size() < 2) return;
					const std::size_t& set_y{ matrix_.size() / 2 };
					const std::size_t& set_x{ matrix_[0].size() / 2 };
					matrix_[set_y - 1][set_x - 1] = white_;
					matrix_[set_y - 1][set_x] = black_;
					matrix_[set_y][set_x - 1] = black_;
					matrix_[set_y][set_x] = white_;
				}
				template<typename Matrix_Int_>
				class Reversi {
				public:
					//コンストラクタ
					constexpr Reversi() noexcept = default;
					template<typename Matrix_>
					constexpr explicit Reversi(Matrix_& matrix_, const Matrix_Int_ black_ = 2, const Matrix_Int_ white_ = 1) noexcept {
						create(matrix_, black_, white_);
					}
					template<typename Matrix_>
					constexpr void create(Matrix_& matrix_, const Matrix_Int_ black_ = 2, const Matrix_Int_ white_ = 1) noexcept {
						createReversi(matrix_, black_, white_);
					}
				};

			}

			namespace data {

				enum :std::size_t {
					shogi_empty,
					shogi_next_place1,
					shogi_next_place2,
					//王将&玉将
					shogi_osho,
					shogi_gyokusho,
					//金将
					shogi_kinsho1,
					shogi_kinsho2,

					//ここより先は成る。----------------------------------------

					//飛車
					shogi_hisha1,
					shogi_ryuo1,
					shogi_hisha2,
					shogi_ryuo2,
					//角行
					shogi_kakugyo1,
					shogi_ryuma1,
					shogi_kakugyo2,
					shogi_ryuma2,

					//ここより先は成ると金と同じ動きになる。--------------------

					//銀将
					shogi_ginsho1,
					shogi_narigin1,
					shogi_ginsho2,
					shogi_narigin2,
					//桂馬
					shogi_keima1,
					shogi_narikei1,
					shogi_keima2,
					shogi_narikei2,
					//香車
					shogi_kyosha1,
					shogi_narikyo1,
					shogi_kyosha2,
					shogi_narikyo2,
					//歩兵
					shogi_fuhyo1,
					shogi_tokin1,
					shogi_fuhyo2,
					shogi_tokin2
				};

				template<typename Matrix_Int_>
				constexpr bool isShogiNarikin(const Matrix_Int_ koma_) noexcept {
					if (koma_ >= shogi_ginsho1 && shogi_narigin1 % 2 == koma_ % 2) return true;
					return false;
				}

			}

			namespace data {

				enum :std::size_t {
					chess_empty,
					chess_king1,
					chess_king2,
					chess_queen1,
					chess_queen2,
					chess_rook1,
					chess_rook2,
					chess_bishop1,
					chess_bishop2,
					chess_knight1,
					chess_knight2,
					chess_pawn1,
					chess_pawn2,
					chess_next_place1,
					chess_next_place2
				};

			}


			namespace stl {

				template<typename Matrix_>
				constexpr void createChess(Matrix_& matrix_) noexcept {
					if (matrix_.size() < 2) return;

					for (std::size_t i{}; i < matrix_[1].size(); ++i)
						matrix_[1][i] = dtl::generator::boardGame::data::chess_pawn2;
					for (std::size_t i{}; i < matrix_[matrix_.size() - 2].size(); ++i)
						matrix_[matrix_.size() - 2][i] = dtl::generator::boardGame::data::chess_pawn1;

					if (matrix_[0].size() < 4) return;
					matrix_[0][0] = dtl::generator::boardGame::data::chess_rook2;
					matrix_[0][matrix_[0].size() - 1] = dtl::generator::boardGame::data::chess_rook2;
					matrix_[0][1] = dtl::generator::boardGame::data::chess_knight2;
					matrix_[0][matrix_[0].size() - 2] = dtl::generator::boardGame::data::chess_knight2;
					matrix_[0][2] = dtl::generator::boardGame::data::chess_bishop2;
					matrix_[0][matrix_[0].size() - 3] = dtl::generator::boardGame::data::chess_bishop2;
					matrix_[0][3] = dtl::generator::boardGame::data::chess_queen2;
					matrix_[0][matrix_[0].size() - 4] = dtl::generator::boardGame::data::chess_king2;
					if (matrix_[matrix_.size() - 1].size() < 4) return;
					matrix_[matrix_.size() - 1][0] = dtl::generator::boardGame::data::chess_rook1;
					matrix_[matrix_.size() - 1][matrix_[matrix_.size() - 1].size() - 1] = dtl::generator::boardGame::data::chess_rook1;
					matrix_[matrix_.size() - 1][1] = dtl::generator::boardGame::data::chess_knight1;
					matrix_[matrix_.size() - 1][matrix_[matrix_.size() - 1].size() - 2] = dtl::generator::boardGame::data::chess_knight1;
					matrix_[matrix_.size() - 1][2] = dtl::generator::boardGame::data::chess_bishop1;
					matrix_[matrix_.size() - 1][matrix_[matrix_.size() - 1].size() - 3] = dtl::generator::boardGame::data::chess_bishop1;
					matrix_[matrix_.size() - 1][3] = dtl::generator::boardGame::data::chess_queen1;
					matrix_[matrix_.size() - 1][matrix_[matrix_.size() - 1].size() - 4] = dtl::generator::boardGame::data::chess_king1;
				}
				template<typename Matrix_Int_>
				class Chess {
				public:
					//コンストラクタ
					constexpr Chess() noexcept = default;
					template<typename Matrix_>
					constexpr explicit Chess(Matrix_& matrix_) noexcept {
						create(matrix_);
					}
					template<typename Matrix_>
					constexpr void create(Matrix_& matrix_) noexcept {
						dtl::generator::boardGame::stl::createChess(matrix_);
					}
				};
			}

			namespace normal {

				template<typename Matrix_>
				constexpr void createChess(Matrix_& matrix_, const std::size_t  x_, const std::size_t y_) noexcept {
					if (y_ < 2) return;

					for (std::size_t col{}; col < x_; ++col)
						matrix_[1][col] = dtl::generator::boardGame::data::chess_pawn2;
					for (std::size_t col{}; col < x_; ++col)
						matrix_[y_ - 2][col] = dtl::generator::boardGame::data::chess_pawn1;

					if (x_ < 4) return;
					matrix_[0][0] = dtl::generator::boardGame::data::chess_rook2;
					matrix_[0][x_ - 1] = dtl::generator::boardGame::data::chess_rook2;
					matrix_[0][1] = dtl::generator::boardGame::data::chess_knight2;
					matrix_[0][x_ - 2] = dtl::generator::boardGame::data::chess_knight2;
					matrix_[0][2] = dtl::generator::boardGame::data::chess_bishop2;
					matrix_[0][x_ - 3] = dtl::generator::boardGame::data::chess_bishop2;
					matrix_[0][3] = dtl::generator::boardGame::data::chess_queen2;
					matrix_[0][x_ - 4] = dtl::generator::boardGame::data::chess_king2;
					matrix_[y_ - 1][0] = dtl::generator::boardGame::data::chess_rook1;
					matrix_[y_ - 1][x_ - 1] = dtl::generator::boardGame::data::chess_rook1;
					matrix_[y_ - 1][1] = dtl::generator::boardGame::data::chess_knight1;
					matrix_[y_ - 1][x_ - 2] = dtl::generator::boardGame::data::chess_knight1;
					matrix_[y_ - 1][2] = dtl::generator::boardGame::data::chess_bishop1;
					matrix_[y_ - 1][x_ - 3] = dtl::generator::boardGame::data::chess_bishop1;
					matrix_[y_ - 1][3] = dtl::generator::boardGame::data::chess_queen1;
					matrix_[y_ - 1][x_ - 4] = dtl::generator::boardGame::data::chess_king1;
				}
				template<typename Matrix_Int_>
				class Chess {
				public:
					//コンストラクタ
					constexpr Chess() noexcept = default;
					template<typename Matrix_>
					constexpr explicit Chess(Matrix_& matrix_, const std::size_t x_, const std::size_t y_) noexcept {
						create(matrix_, x_, y_);
					}
					template<typename Matrix_>
					constexpr void create(Matrix_& matrix_, const std::size_t x_, const std::size_t y_) noexcept {
						dtl::generator::boardGame::normal::createChess(matrix_, x_, y_);
					}
				};
			}





		} //namespace
	}
}

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <algorithm>
#include <utility>
#include <memory>

namespace dtl {
	namespace generator {
		namespace boardGame {
			namespace array {

				struct KnightTourNode {
					std::int_fast32_t row{}, col{};
					bool visited{};
					std::vector<std::shared_ptr<KnightTourNode>> next{};

					KnightTourNode(const std::int_fast32_t row_, const std::int_fast32_t col_) noexcept : row(row_), col(col_), visited(false) {}
				};

				class KnightTourIsUnvisited {
				public:
					bool operator()(const std::shared_ptr<KnightTourNode>& node_) const noexcept { return !node_->visited; }
				};

				class KnightTourIsVisited {
				public:
					bool operator()(const std::shared_ptr<KnightTourNode>& node_) const noexcept { return node_->visited; }
				};
				//等しくない時
				class KnightTourNotEqualUnvisited {
				private:
					std::size_t counter{};
				public:
					KnightTourNotEqualUnvisited(const std::shared_ptr<KnightTourNode>& node_) noexcept : counter(static_cast<std::size_t>(std::count_if(node_->next.begin(), node_->next.end(), KnightTourIsUnvisited()))) { }
					bool operator()(const std::shared_ptr<KnightTourNode>& node_) const noexcept {
						return static_cast<std::size_t>(std::count_if(node_->next.begin(), node_->next.end(), KnightTourIsUnvisited())) != counter;
					}
				};
				//動かない時
				class KnightTourLessMovable {
				public:
					bool operator()(const std::shared_ptr<KnightTourNode>& node1_, const std::shared_ptr<KnightTourNode>& node2_) const noexcept {
						return std::count_if(node1_->next.begin(), node1_->next.end(), KnightTourIsUnvisited()) < std::count_if(node2_->next.begin(), node2_->next.end(), KnightTourIsUnvisited());
					}
				};
				//騎士の巡歴
				template<typename Matrix_Int_>
				class KnightTour {
				private:

					void search(const std::int_fast32_t x_, const std::int_fast32_t y_, const bool is_closed_, std::shared_ptr<KnightTourNode>& node_, std::vector<std::shared_ptr<KnightTourNode>>& best_tour_, std::vector<std::shared_ptr<KnightTourNode>>& tour_) const noexcept {
						if (node_->visited) return;
						if (best_tour_.size() == static_cast<std::size_t>(y_ * x_)) {
							if (!is_closed_) return;
							//騎士の周遊
							if (std::find(best_tour_.back()->next.begin(), best_tour_.back()->next.end(), best_tour_.front()) != best_tour_.back()->next.end()) return;
							for (auto&& p : best_tour_.back()->next) {
								auto&& q{ std::find(best_tour_.begin(), best_tour_.end(), p) + 1 };
								auto&& r{ std::find(best_tour_.front()->next.begin(), best_tour_.front()->next.end(), *q) };
								if (r != best_tour_.front()->next.end()) {
									std::reverse(q, best_tour_.end());
									return;
								}
							}
							best_tour_.clear();
							return;
						}
						node_->visited = true;
						tour_.emplace_back(node_);
						if (best_tour_.size() < tour_.size()) best_tour_ = tour_;
						//Warnsdorffのアルゴリズム
						std::sort(node_->next.begin(), node_->next.end(), KnightTourLessMovable());

						if (node_ == nullptr) return;

						//
						std::vector<std::shared_ptr<KnightTourNode>> next(node_->next);
						next.erase(std::remove_if(next.begin(), next.end(), KnightTourIsVisited()), next.end());
						if (!next.empty())
							next.erase(std::remove_if(next.begin(), next.end(), KnightTourNotEqualUnvisited(next.front())), next.end());
						for (auto&& p : next)
							search(x_, y_, is_closed_, p, best_tour_, tour_);
						node_->visited = false;
						tour_.pop_back();
					}

					std::size_t tour(const std::int_fast32_t x_, const std::int_fast32_t y_, const bool is_closed_, const std::size_t start_pos_, std::vector<std::shared_ptr<KnightTourNode>>& nodes_, std::vector<std::shared_ptr<KnightTourNode>>& best_tour_) const noexcept {
						//桂馬飛びの位置を格納する
						constexpr std::array<std::pair<std::int_fast32_t, std::int_fast32_t>, 8> moves{ {
									std::make_pair(2, 1),std::make_pair(1, 2),std::make_pair(2, -1),std::make_pair(1, -2),
									std::make_pair(-2, 1),std::make_pair(-1, 2),std::make_pair(-2, -1),std::make_pair(-1, -2)
							} };

						//ノードの初期化
						for (std::int_fast32_t i{}; i < y_; ++i)
							for (std::int_fast32_t j{}; j < x_; ++j)
								nodes_.emplace_back(std::make_shared<KnightTourNode>(i, j));

						nodes_.shrink_to_fit();

						for (auto&& p : nodes_)
							for (auto&& q : moves) {
								std::int_fast32_t r{ p->row + q.first };
								std::int_fast32_t c{ p->col + q.second };
								if (r >= 0 && r < y_ && c >= 0 && c < x_) p->next.emplace_back(nodes_[r*x_ + c]);
							}
						//Schwenkの定理
						if (is_closed_ && (y_ * x_ % 2 == 1 || ((std::min)(y_, x_) == 2 || (std::min)(y_, x_) == 4)
							|| ((std::min)(y_, x_) == 3 && ((std::max)(y_, x_) == 4 || (std::max)(y_, x_) == 6 || (std::max)(y_, x_) == 8)))) return 0;
						if (!is_closed_ && y_ * x_ % 2 == 1 && start_pos_ % 2 == 1) return 0;

						//探索
						std::vector<std::shared_ptr<KnightTourNode>> tour_;
						search(x_, y_, is_closed_, nodes_[start_pos_], best_tour_, tour_);
						return best_tour_.size();
					}
					//巡歴をマップ上に記録する
					template<typename Matrix_>
					void setTour(Matrix_& matrix_, std::vector<std::shared_ptr<KnightTourNode>>& best_tour_, const Matrix_Int_ mod_value_ = 0) const noexcept {
						Matrix_Int_ counter{};
						if (mod_value_ < 2)
							for (const auto& i : best_tour_) {
								if (i == nullptr) continue;
								matrix_[i->row][i->col] = counter;
								++counter;
							}
						else for (const auto& i : best_tour_) {
							if (i == nullptr) continue;
							matrix_[i->row][i->col] = counter;
							++counter;
							if (counter >= mod_value_) counter = 0;
						}
					}

				public:
					//生成
					template<typename Matrix_>
					bool create(Matrix_& matrix_, const std::size_t x_, const std::size_t y_, const std::size_t start_x_ = 0, const std::size_t start_y_ = 0, const bool is_closed_ = false, const Matrix_Int_ mod_value_ = 0) const noexcept {
						std::vector<std::shared_ptr<KnightTourNode>> nodes;
						std::vector<std::shared_ptr<KnightTourNode>> best_tour;

						if (tour(static_cast<std::int_fast32_t>(x_), static_cast<std::int_fast32_t>(y_), is_closed_, start_y_*x_ + start_x_, nodes, best_tour) < y_ * x_) return false;
						setTour(matrix_, best_tour, mod_value_);
						return true;
					}
					//コンストラクタ
					constexpr KnightTour() noexcept = default;
					template<typename Matrix_>
					constexpr explicit KnightTour(Matrix_& matrix_, const std::size_t x_, const std::size_t y_, const std::size_t start_x_ = 0, const std::size_t start_y_ = 0, const bool is_closed_ = false, const Matrix_Int_ mod_value_ = 0) noexcept {
						create(matrix_, x_, y_, start_x_, start_y_, is_closed_, mod_value_);
					}

				};

			} //namespace
		}
	}
}


#endif //Included Dungeon Template Library