#pragma once
#include "polygon_dx.h"

class enemy final : public polygon_dx {
public:
    // �R���X�g���N�^
    enemy();
    enemy(const enemy&) = default; // �R�s�[
    enemy(enemy&&) = default; // ���[�u

    // �f�X�g���N�^
    virtual ~enemy() = default;

    enemy& operator =(const enemy&) = default; // �R�s�[
    enemy& operator =(enemy&&) = default; // ���[�u

    void update() override;
    void render() override;

    void set_hit(const bool hit);
    bool get_hit() const { return hit; }

private:
    double angle_y;
    bool hit;
    bool end;
};
